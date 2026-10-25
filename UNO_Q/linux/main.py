#!/usr/bin/env python3
"""
Batman — Linux-side controller
- Opens serial to Arduino UNO and sends simple commands
- Provides a minimal interactive shell and an optional demo loop

Usage:
  python UNO_Q/linux/main.py [--port /dev/ttyACM0] [--baud 115200] [--demo]
"""

import argparse
import sys
import time
from typing import Optional, List

try:
    import serial  # type: ignore
    import serial.tools.list_ports  # type: ignore
except Exception as e:  # pragma: no cover
    print("ERROR: pyserial is required. Install with: pip install pyserial", file=sys.stderr)
    raise


def find_default_port() -> Optional[str]:
    # Prefer ACM, then USB
    candidates: List[str] = []
    for p in serial.tools.list_ports.comports():
        name = p.device
        if "ttyACM" in name:
            candidates.append(name)
    for p in serial.tools.list_ports.comports():
        name = p.device
        if "ttyUSB" in name:
            candidates.append(name)
    return candidates[0] if candidates else None


class App:
    def __init__(self, port: Optional[str], baud: int, demo: bool) -> None:
        self.port = port or find_default_port()
        if not self.port:
            raise RuntimeError("No serial port found. Pass --port explicitly.")
        self.baud = baud
        self.demo = demo
        self.ser: Optional[serial.Serial] = None

    def open(self) -> None:
        self.ser = serial.Serial(self.port, self.baud, timeout=0.05)
        # Allow Arduino reset on serial-open
        time.sleep(2.0)
        self._drain()

    def close(self) -> None:
        if self.ser and self.ser.is_open:
            self.ser.close()
            self.ser = None

    def _send(self, line: str) -> None:
        if not self.ser:
            raise RuntimeError("Serial not open")
        data = (line.strip() + "\n").encode("ascii", errors="ignore")
        self.ser.write(data)

    def _drain(self) -> None:
        if not self.ser:
            return
        try:
            while True:
                b = self.ser.readline()
                if not b:
                    break
                sys.stdout.write(b.decode("utf-8", errors="ignore"))
        except Exception:
            pass

    def interactive(self) -> None:
        print(f"Connected on {self.port} @ {self.baud}. Type 'help' for commands, Ctrl+C to exit.")
        print("Examples: S | F 120 | B 120 | L 120 | R 120 | M L=100 R=80 | SB 90 | SA 45 | SG 10")
        while True:
            try:
                self._drain()
                cmd = input("> ").strip()
                if not cmd:
                    continue
                if cmd.lower() in ("quit", "exit"):
                    break
                if cmd.lower() == "help":
                    print("S | F [spd] | B [spd] | L [spd] | R [spd] | M L=<v> R=<v> | SB <a> | SA <a> | SG <a>")
                    continue
                self._send(cmd)
                # brief wait and print any response
                time.sleep(0.05)
                self._drain()
            except KeyboardInterrupt:
                print()
                break

    def demo_loop(self) -> None:
        print("Running demo loop: forward 1s, stop 0.5s, left 0.5s, right 0.5s, stop.")
        seq = [
            ("F 120", 1.0),
            ("S", 0.5),
            ("L 120", 0.5),
            ("R 120", 0.5),
            ("S", 0.5),
        ]
        for cmd, dt in seq:
            self._send(cmd)
            time.sleep(dt)
            self._drain()

    def run(self) -> None:
        self.open()
        try:
            if self.demo:
                self.demo_loop()
            self.interactive()
        finally:
            self.close()


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=str, default=None, help="Serial port (e.g., /dev/ttyACM0)")
    ap.add_argument("--baud", type=int, default=115200, help="Serial baud rate")
    ap.add_argument("--demo", action="store_true", help="Run a short demo sequence before interactive shell")
    args = ap.parse_args()
    app = App(port=args.port, baud=args.baud, demo=args.demo)
    app.run()


if __name__ == "__main__":
    main()

