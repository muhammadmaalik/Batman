/*
  Batman — Arduino standalone base sketch
  - Same pinout as UNO_Q/mcu/robot_mcu.ino
  - Provides simple serial control; can run without a Linux host
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

Servo servoBase, servoArm, servoGrip;

struct MotorChannel { int in1; int in2; int pwm; };
MotorChannel M_LEFT_FRONT  = { M1_AIN1, M1_AIN2, M1_PWMA };
MotorChannel M_RIGHT_FRONT = { M1_BIN1, M1_BIN2, M1_PWMB };
MotorChannel M_LEFT_REAR   = { M2_AIN1, M2_AIN2, M2_PWMA };
MotorChannel M_RIGHT_REAR  = { M2_BIN1, M2_BIN2, M2_PWMB };

void driveChannel(const MotorChannel& m, int speed) {
  int mag = speed; if (mag < 0) mag = -mag; if (mag > 255) mag = 255;
  if (speed > 0) { digitalWrite(m.in1, HIGH); digitalWrite(m.in2, LOW); }
  else if (speed < 0) { digitalWrite(m.in1, LOW); digitalWrite(m.in2, HIGH); }
  else { digitalWrite(m.in1, LOW); digitalWrite(m.in2, LOW); }
  analogWrite(m.pwm, mag);
}
void setLR(int l, int r) {
  driveChannel(M_LEFT_FRONT, l); driveChannel(M_LEFT_REAR, l);
  driveChannel(M_RIGHT_FRONT, r); driveChannel(M_RIGHT_REAR, r);
}
void stopAll() { setLR(0, 0); }

static String rxLine;
int parseIntAfter(const String& s, const String& k, int fb) {
  int i = s.indexOf(k); if (i < 0) return fb; i += k.length();
  while (i < (int)s.length() && (s[i] == ' ' || s[i] == '=')) i++;
  int sign = 1; if (i < (int)s.length() && s[i] == '-') { sign = -1; i++; }
  long v = 0; bool any = false;
  while (i < (int)s.length() && isDigit(s[i])) { v = v*10 + (s[i]-'0'); i++; any = true; }
  if (!any) return fb; int iv = (int)(v*sign);
  if (iv > 255) iv = 255; if (iv < -255) iv = -255; return iv;
}
void handleLine(const String& raw) {
  String s = raw; s.trim(); if (!s.length()) return;
  String up = s; up.toUpperCase();
  if (up.startsWith("M")) { int l = parseIntAfter(s,"L",0), r = parseIntAfter(s,"R",0); setLR(l,r); Serial.println(F("OK M")); return; }
  if (up == "S") { stopAll(); Serial.println(F("OK S")); return; }
  if (up[0]=='F'||up[0]=='B'||up[0]=='L'||up[0]=='R') {
    int sp = parseIntAfter(s, String(up[0]), 120); if (sp<0) sp=0; if (sp>255) sp=255;
    if (up[0]=='F') setLR(sp,sp); if (up[0]=='B') setLR(-sp,-sp);
    if (up[0]=='L') setLR(-sp,sp); if (up[0]=='R') setLR(sp,-sp);
    Serial.println(F("OK MOVE")); return;
  }
  if (up.startsWith("SB")) { int v=parseIntAfter(s,"SB",90); if(v<0)v=0;if(v>180)v=180; servoBase.write(v); Serial.println(F("OK SB")); return; }
  if (up.startsWith("SA")) { int v=parseIntAfter(s,"SA",90); if(v<0)v=0;if(v>180)v=180; servoArm.write(v);  Serial.println(F("OK SA")); return; }
  if (up.startsWith("SG")) { int v=parseIntAfter(s,"SG",90); if(v<0)v=0;if(v>180)v=180; servoGrip.write(v); Serial.println(F("OK SG")); return; }
  Serial.println(F("ERR"));
}

void setup() {
  pinMode(M1_AIN1, OUTPUT); pinMode(M1_AIN2, OUTPUT);
  pinMode(M1_BIN1, OUTPUT); pinMode(M1_BIN2, OUTPUT);
  pinMode(M2_AIN1, OUTPUT); pinMode(M2_AIN2, OUTPUT);
  pinMode(M2_BIN1, OUTPUT); pinMode(M2_BIN2, OUTPUT);
  digitalWrite(M1_AIN1, LOW); digitalWrite(M1_AIN2, LOW);
  digitalWrite(M1_BIN1, LOW); digitalWrite(M1_BIN2, LOW);
  digitalWrite(M2_AIN1, LOW); digitalWrite(M2_AIN2, LOW);
  digitalWrite(M2_BIN1, LOW); digitalWrite(M2_BIN2, LOW);
  servoBase.attach(SERVO_BASE); servoArm.attach(SERVO_ARM); servoGrip.attach(SERVO_GRIP);
  servoBase.write(90); servoArm.write(90); servoGrip.write(90);
  Serial.begin(115200);
  Serial.println(F("BATMAN BASE READY"));
  // Gentle nudge forward to prove life (safe if wheels lifted)
  setLR(80, 80); delay(500); stopAll();
}

void loop() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c=='\r') continue;
    if (c=='\n') { handleLine(rxLine); rxLine=""; }
    else if (rxLine.length() < 96) rxLine += c;
  }
}

