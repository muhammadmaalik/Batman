# Batman

SBUHacks 2026 robotics scaffold for Batman — an Arduino UNO–driven rover with:

- Dual HW-166 (TB6612FNG) motor drivers powering four DC motors
- Three servos (base, arm, gripper)
- A Linux-side controller (Python) with optional OpenCV vision and simple serial control

This repository is a clean, starter scaffold — no model weights or binaries are included.

## Repository layout

- `docs/DEVPOST.md` — Devpost-ready writeup template
- `docs/WIRING.md` — Wiring and pinout reference (UNO + HW-166 + servos)
- `UNO_Q/mcu/robot_mcu.ino` — Arduino UNO sketch speaking a simple serial protocol
- `UNO_Q/linux/main.py` — Linux-side app: serial control loop (`App.run`), no broken `user_setup`
- `UNO_Q/README.md` — Notes for Linux + UNO quickstart
- `Vision/detect_drive.py` — Minimal color-based vision that can issue drive commands
- `Arduino/robot_base/robot_base.ino` — Standalone Arduino base sketch (no host required)
- `tests/motor_all_forward.ino` — Sanity test: spin all motors forward
- `tests/servo_sweep.ino` — Sanity test: sweep all servos
- `.gitignore` — Ignore caches, build artifacts, and ML weights
- `requirements.txt` — Python deps for Linux-side tools

## Quickstart

### 1) Flash Arduino

- Open `UNO_Q/mcu/robot_mcu.ino` (or `Arduino/robot_base/robot_base.ino`) in the Arduino IDE
- Board: Arduino UNO; Programmer: default
- Upload

### 2) Linux control (optional)

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
python UNO_Q/linux/main.py
```

By default the app auto-detects `/dev/ttyACM*` or `/dev/ttyUSB*`. Override with `--port`.

### 3) Vision assist (optional)

```bash
python Vision/detect_drive.py --port /dev/ttyACM0
```

This demo uses basic color segmentation (no ML weights) to issue coarse drive nudges.

## Wiring

See `docs/WIRING.md` for the full pin map:

- HW-166 #1: `PWMA 3`, `AIN1 2`, `AIN2 4`, `PWMB 5`, `BIN1 A2`, `BIN2 A3`
- HW-166 #2: `PWMA 6`, `AIN1 7`, `AIN2 8`, `PWMB 11`, `BIN1 12`, `BIN2 13`
- Servos: `Base 9`, `Arm 10`, `Grip A0`
- `VCC/STBY` at `3.3V`; common `GND`

## Safety

- Lift wheels off the table for first motor tests
- Start at low PWM values and verify motor direction before increasing speed
- Keep clear of servos’ range to avoid pinching

## License

MIT (see `LICENSE`)