# UNO_Q — Linux + Arduino Quickstart

This folder contains:

- `mcu/robot_mcu.ino` — Arduino UNO sketch that controls motors and servos and speaks a simple serial protocol
- `linux/main.py` — Python app for sending commands over serial; includes an interactive shell

## Prerequisites

- Arduino IDE installed
- Python 3.9+ on Linux
- USB cable to connect Arduino UNO

## Flash the MCU

1. Open `UNO_Q/mcu/robot_mcu.ino` in Arduino IDE
2. Select Board: `Arduino UNO`
3. Upload

## Linux-side app

Set up a virtual environment and run:

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
python UNO_Q/linux/main.py --demo
```

If auto-detection fails, pass `--port` explicitly (e.g., `/dev/ttyACM0`).

## Serial protocol (line-oriented)

- `S` — stop all motors
- `F [speed]` — forward (0..255)
- `B [speed]` — backward (0..255)
- `L [speed]` — pivot left
- `R [speed]` — pivot right
- `M L=<val> R=<val>` — set left/right speeds directly (-255..255)
- `SB <angle>` — base servo 0..180
- `SA <angle>` — arm servo 0..180
- `SG <angle>` — grip servo 0..180

Outputs `OK ...` or `ERR` for invalid commands.

