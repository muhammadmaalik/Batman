# Batman — SBUHacks 2026 Devpost Writeup

## Inspiration
Briefly describe the problem space and what motivated Batman.

## What it does
- Arduino UNO rover with dual HW-166 (TB6612FNG) motor drivers
- Three-servo manipulator (base, arm, gripper)
- Linux companion app for serial control and simple OpenCV vision assist

## How we built it
- Hardware: Arduino UNO, 2× HW-166 motor drivers, 4× DC motors, 3× servos
- Firmware: Arduino sketches (`UNO_Q/mcu/robot_mcu.ino`, `Arduino/robot_base/robot_base.ino`)
- Software: Python 3 (`UNO_Q/linux/main.py`, `Vision/detect_drive.py`)
- No ML weights or binaries checked in — pure classical CV demo for hackathon portability

## Challenges we ran into
- Reliable power and common ground across drivers, servos, and UNO
- Consistent serial framing across platforms
- Tuning PWM ramps to avoid brownouts when all motors start

## Accomplishments that we're proud of
- Clean, reusable scaffold teammates can iterate on
- Simple testing sketches to de-risk wiring early
- Documented pinout and wiring for fast bring-up

## What we learned
- Serial protocols should be explicit, robust, and debuggable
- Current spikes from DC motors require careful ramping and power decoupling
- Classical CV can still be effective for constrained tasks

## What's next for Batman
- Higher-level autonomy (path planning, obstacle avoidance)
- Gripper feedback and safety interlocks
- ROS2 bridge for richer sensor and control integration

## How to run
1. Flash `UNO_Q/mcu/robot_mcu.ino` to Arduino UNO
2. On Linux:
   ```bash
   python3 -m venv .venv && source .venv/bin/activate
   pip install -r requirements.txt
   python UNO_Q/linux/main.py --port /dev/ttyACM0
   ```
3. Optional vision assist:
   ```bash
   python Vision/detect_drive.py --port /dev/ttyACM0
   ```

