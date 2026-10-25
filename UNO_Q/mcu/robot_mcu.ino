/*
  Batman — Arduino UNO MCU sketch
  - Controls two HW-166 (TB6612FNG) motor drivers (4 DC motors total)
  - Controls three servos: base, arm, grip
  - Exposes a simple line-oriented serial protocol for host control
    Examples:
      S                    -> stop all motors
      F 120                -> drive forward at speed 120 (0..255)
      B 100                -> drive backward at speed 100
      L 140                -> pivot left
      R 140                -> pivot right
      M L=120 R=80         -> set left/right speeds directly
      SB 90                -> set Base servo to 90 deg
      SA 45                -> set Arm servo to 45 deg
      SG 10                -> set Grip servo to 10 deg
*/

#include <Servo.h>

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

Servo servoBase;
Servo servoArm;
Servo servoGrip;

struct MotorChannel {
  int in1;
  int in2;
  int pwm;
};

MotorChannel M_LEFT_FRONT  = { M1_AIN1, M1_AIN2, M1_PWMA }; // Assumed mapping
MotorChannel M_RIGHT_FRONT = { M1_BIN1, M1_BIN2, M1_PWMB };
MotorChannel M_LEFT_REAR   = { M2_AIN1, M2_AIN2, M2_PWMA };
MotorChannel M_RIGHT_REAR  = { M2_BIN1, M2_BIN2, M2_PWMB };

void driveChannel(const MotorChannel& m, int speed /* -255..255 */) {
  int mag = speed;
  if (mag < 0) mag = -mag;
  if (mag > 255) mag = 255;

  if (speed > 0) {
    digitalWrite(m.in1, HIGH);
    digitalWrite(m.in2, LOW);
  } else if (speed < 0) {
    digitalWrite(m.in1, LOW);
    digitalWrite(m.in2, HIGH);
  } else {
    // Brake low (coast)
    digitalWrite(m.in1, LOW);
    digitalWrite(m.in2, LOW);
  }
  analogWrite(m.pwm, mag);
}

void setLeftRight(int leftSpeed, int rightSpeed) {
  driveChannel(M_LEFT_FRONT, leftSpeed);
  driveChannel(M_LEFT_REAR, leftSpeed);
  driveChannel(M_RIGHT_FRONT, rightSpeed);
  driveChannel(M_RIGHT_REAR, rightSpeed);
}

void stopAll() {
  setLeftRight(0, 0);
}

static String rxLine;

int parseIntAfter(const String& s, const String& key, int fallback, bool* found) {
  int idx = s.indexOf(key);
  if (idx < 0) { if (found) *found = false; return fallback; }
  idx += key.length();
  // skip spaces and '='
  while (idx < (int)s.length() && (s[idx] == ' ' || s[idx] == '=')) idx++;
  int sign = 1;
  if (idx < (int)s.length() && s[idx] == '-') { sign = -1; idx++; }
  long val = 0;
  bool any = false;
  while (idx < (int)s.length() && isDigit(s[idx])) {
    val = val * 10 + (s[idx] - '0');
    idx++; any = true;
  }
  if (!any) { if (found) *found = false; return fallback; }
  if (found) *found = true;
  int iv = (int)(val * sign);
  if (iv > 255) iv = 255;
  if (iv < -255) iv = -255;
  return iv;
}

void handleLine(const String& line) {
  String cmd = line;
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.length() == 0) return;

  if (cmd.startsWith("M")) {
    bool fL=false, fR=false;
    int l = parseIntAfter(line, "L", 0, &fL);
    int r = parseIntAfter(line, "R", 0, &fR);
    if (fL || fR) {
      setLeftRight(l, r);
      Serial.println(F("OK M"));
      return;
    }
  }

  if (cmd[0] == 'S' && (cmd.length() == 1 || cmd[1] == ' ')) {
    // Stop or Servo
    if (cmd.startsWith("SB")) {
      int v = parseIntAfter(line, "SB", 90, nullptr);
      if (v < 0) v = 0; if (v > 180) v = 180;
      servoBase.write(v);
      Serial.println(F("OK SB"));
      return;
    } else if (cmd.startsWith("SA")) {
      int v = parseIntAfter(line, "SA", 90, nullptr);
      if (v < 0) v = 0; if (v > 180) v = 180;
      servoArm.write(v);
      Serial.println(F("OK SA"));
      return;
    } else if (cmd.startsWith("SG")) {
      int v = parseIntAfter(line, "SG", 90, nullptr);
      if (v < 0) v = 0; if (v > 180) v = 180;
      servoGrip.write(v);
      Serial.println(F("OK SG"));
      return;
    } else if (cmd == "S") {
      stopAll();
      Serial.println(F("OK S"));
      return;
    }
  }

  // Cardinal moves with optional speed
  if (cmd[0] == 'F' || cmd[0] == 'B' || cmd[0] == 'L' || cmd[0] == 'R') {
    int sp = parseIntAfter(line, String(cmd[0]), 120, nullptr);
    if (sp < 0) sp = 0; if (sp > 255) sp = 255;
    if (cmd[0] == 'F') setLeftRight(sp, sp);
    if (cmd[0] == 'B') setLeftRight(-sp, -sp);
    if (cmd[0] == 'L') setLeftRight(-sp, sp);
    if (cmd[0] == 'R') setLeftRight(sp, -sp);
    Serial.println(F("OK MOVE"));
    return;
  }

  Serial.println(F("ERR"));
}

void setup() {
  pinMode(M1_AIN1, OUTPUT);
  pinMode(M1_AIN2, OUTPUT);
  pinMode(M1_BIN1, OUTPUT);
  pinMode(M1_BIN2, OUTPUT);
  pinMode(M2_AIN1, OUTPUT);
  pinMode(M2_AIN2, OUTPUT);
  pinMode(M2_BIN1, OUTPUT);
  pinMode(M2_BIN2, OUTPUT);

  // Safe defaults
  digitalWrite(M1_AIN1, LOW);
  digitalWrite(M1_AIN2, LOW);
  digitalWrite(M1_BIN1, LOW);
  digitalWrite(M1_BIN2, LOW);
  digitalWrite(M2_AIN1, LOW);
  digitalWrite(M2_AIN2, LOW);
  digitalWrite(M2_BIN1, LOW);
  digitalWrite(M2_BIN2, LOW);

  servoBase.attach(SERVO_BASE);
  servoArm.attach(SERVO_ARM);
  servoGrip.attach(SERVO_GRIP);

  servoBase.write(90);
  servoArm.write(90);
  servoGrip.write(90);

  Serial.begin(115200);
  while (!Serial) { /* wait on native USB boards; UNO will skip */ }
  Serial.println(F("BATMAN READY"));
}

void loop() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      handleLine(rxLine);
      rxLine = "";
    } else {
      if (rxLine.length() < 96) rxLine += c; // simple bound
    }
  }
}

