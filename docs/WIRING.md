# Wiring and Pinout — Batman (Arduino UNO + HW-166 + Servos)

All grounds must be common. Power the logic side and `STBY` at 3.3V as specified here. Motor power (VM/VBAT) should follow your motor specs and driver limits — keep separate from logic 3.3V and share ground.

## Summary

- Microcontroller: Arduino UNO
- Motor drivers: 2× HW-166 (TB6612FNG)
- Servos: Base, Arm, Grip
- Logic VCC/STBY: 3.3V
- Common GND across UNO, drivers, servos, and power source

## Pin map

HW-166 #1 (TB6612FNG):

- `PWMA -> D3`
- `AIN1 -> D2`
- `AIN2 -> D4`
- `PWMB -> D5`
- `BIN1 -> A2` (digital)
- `BIN2 -> A3` (digital)

HW-166 #2 (TB6612FNG):

- `PWMA -> D6`
- `AIN1 -> D7`
- `AIN2 -> D8`
- `PWMB -> D11`
- `BIN1 -> D12`
- `BIN2 -> D13`

Servos:

- `Base -> D9`
- `Arm  -> D10`
- `Grip -> A0` (digital 14)

Power:

- `VCC` (logic) -> `3.3V` on UNO
- `STBY` -> `3.3V` (enable the driver)
- `GND` -> common ground

## Arduino constants (for reference)

```cpp
// HW-166 #1
const int M1_PWMA = 3;
const int M1_AIN1 = 2;
const int M1_AIN2 = 4;
const int M1_PWMB = 5;
const int M1_BIN1 = A2; // digital
const int M1_BIN2 = A3; // digital

// HW-166 #2
const int M2_PWMA = 6;
const int M2_AIN1 = 7;
const int M2_AIN2 = 8;
const int M2_PWMB = 11;
const int M2_BIN1 = 12;
const int M2_BIN2 = 13;

// Servos
const int SERVO_BASE = 9;
const int SERVO_ARM  = 10;
const int SERVO_GRIP = A0; // digital 14
```

## Notes and tips

- Treat analog pins `A0..A5` as digital `14..19` when needed.
- Start motor PWM low (e.g., 90–120) to validate direction before ramping up.
- Tie `STBY` high (3.3V) to enable TB6612; pulling low puts the driver in standby.
- If motors twitch on boot, ensure inputs default LOW and attach PWM only after setup.
- Servos can draw significant current; power-budget accordingly.

## Sanity tests

- Upload `tests/motor_all_forward.ino` — all wheels should spin forward.
- Upload `tests/servo_sweep.ino` — servos should sweep within safe ranges.

