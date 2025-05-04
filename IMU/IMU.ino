#include <Wire.h>
#include "MPU6050.h"

const int RPWM = 3;
const int LPWM = 10;
const int taperpin = 11;

float sensitivity = 5.0;

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();                 // uses SDA=A4, SCL=A5 automatically

  mpu.initialize();
  mpu.setSleepEnabled(false);   // wake the MPU‑6050

  pinMode(LPWM, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(taperpin, OUTPUT);

  if (mpu.testConnection()){
    Serial.println("MPU6050 is connected.");
  } else {
    Serial.println("MPU6050 connection failed.");
  }
}

void loop() {
  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);

  float gyroX = gx / 131.0;     // dps
//  Serial.println(gyroX);

  if (gyroX >  sensitivity) { digitalWrite(LPWM, HIGH); digitalWrite(RPWM, LOW); }
  else if (gyroX < -sensitivity){ digitalWrite(RPWM, HIGH); digitalWrite(LPWM, LOW); }
  else { digitalWrite(RPWM, LOW); digitalWrite(LPWM, LOW); }
}
