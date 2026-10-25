#!/usr/bin/env python3
"""
Batman — Simple vision-assisted driving

Detects a colored target (default: green) and issues coarse drive nudges
to the Arduino over serial.

This is a classical CV demo — no ML weights or binaries included.
"""

import argparse
import time
from typing import Optional, Tuple

import cv2  # type: ignore
import numpy as np  # type: ignore

try:
    import serial  # type: ignore
    import serial.tools.list_ports  # type: ignore
except Exception:
    serial = None  # type: ignore


def find_default_port() -> Optional[str]:
    if serial is None:
        return None
    for p in serial.tools.list_ports.comports():
        if "ttyACM" in p.device or "ttyUSB" in p.device:
            return p.device
    return None


def largest_contour(mask: np.ndarray) -> Optional[np.ndarray]:
    cnts, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not cnts:
        return None
    return max(cnts, key=cv2.contourArea)


def centroid(contour: np.ndarray) -> Optional[Tuple[int, int]]:
    M = cv2.moments(contour)
    if M["m00"] == 0:
        return None
    cx = int(M["m10"] / M["m00"])
    cy = int(M["m01"] / M["m00"])
    return cx, cy


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=str, default=None, help="Serial port (e.g., /dev/ttyACM0)")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--camera", type=int, default=0)
    ap.add_argument("--hsv-low", type=int, nargs=3, default=[35, 70, 70], help="HSV lower bound")
    ap.add_argument("--hsv-high", type=int, nargs=3, default=[85, 255, 255], help="HSV upper bound")
    ap.add_argument("--show", action="store_true", help="Show debug preview windows")
    ap.add_argument("--throttle", type=float, default=0.25, help="Seconds between serial commands")
    ap.add_argument("--fwd-speed", type=int, default=110)
    ap.add_argument("--turn-speed", type=int, default=120)
    args = ap.parse_args()

    ser = None
    if serial is not None:
        port = args.port or find_default_port()
        if port:
            ser = serial.Serial(port, args.baud, timeout=0.02)
            time.sleep(2.0)

    cap = cv2.VideoCapture(args.camera)
    if not cap.isOpened():
        raise RuntimeError("Could not open camera")

    last_send = 0.0
    try:
        while True:
            ok, frame = cap.read()
            if not ok:
                break

            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            lower = np.array(args.hsv_low, dtype=np.uint8)
            upper = np.array(args.hsv_high, dtype=np.uint8)
            mask = cv2.inRange(hsv, lower, upper)
            mask = cv2.medianBlur(mask, 5)
            cnt = largest_contour(mask)

            h, w = frame.shape[:2]
            cx = cy = None
            area = 0.0
            if cnt is not None:
                area = cv2.contourArea(cnt)
                c = centroid(cnt)
                if c:
                    cx, cy = c
                    if args.show:
                        cv2.circle(frame, (cx, cy), 5, (0, 0, 255), -1)
                        x, y, ww, hh = cv2.boundingRect(cnt)
                        cv2.rectangle(frame, (x, y), (x + ww, y + hh), (0, 255, 0), 2)

            # Decide action
            now = time.time()
            if ser is not None and now - last_send >= args.throttle:
                if cx is None:
                    ser.write(b"S\n")
                else:
                    offset = (cx - w / 2) / (w / 2)  # -1 .. 1
                    if abs(offset) < 0.15:
                        ser.write(f"F {args.fwd_speed}\n".encode("ascii"))
                    elif offset < 0:
                        ser.write(f"L {args.turn_speed}\n".encode("ascii"))
                    else:
                        ser.write(f"R {args.turn_speed}\n".encode("ascii"))
                last_send = now

            if args.show:
                if cnt is None:
                    txt = "No target"
                else:
                    txt = f"Target @ {cx},{cy} area={int(area)}"
                cv2.putText(frame, txt, (10, 24), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
                cv2.imshow("frame", frame)
                cv2.imshow("mask", mask)
                if cv2.waitKey(1) & 0xFF == 27:
                    break
    finally:
        cap.release()
        if args.show:
            try:
                cv2.destroyAllWindows()
            except Exception:
                pass
        if ser is not None:
            try:
                ser.write(b"S\n")
                ser.close()
            except Exception:
                pass


if __name__ == "__main__":
    main()

