#include <Wire.h>
#include "MPU6050.h" // IMU library, will need to download from online

int RPWM = 3;  // digital Output to motor
int LPWM = 10;

int taperpin = 11; // digital output to motor


float sensitivity = 5.0; // offset for sensitivity
float speed = 0; //initial global speed variable

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize the MPU6050
  mpu.initialize();
  //initialize pins
  pinMode(LPWM, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(taperpin, OUTPUT);

  // Check if the MPU6050 is connected
  if (mpu.testConnection()) {
    Serial.println("MPU6050 is connected.");
  } else {
    Serial.println("MPU6050 connection failed.");
  }
}

void loop() {
  // Read the gyroscope values
  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);

  // Convert to degrees per second
  float gyroX1 = gx / 131.0; // Scale factor for MPU6050
  speed = abs(gyroX1);

  Serial.println(speed); // Print the gyroscope velocity

  // Handle PWM irection
  if (gyroX1 > sensitivity) {
    digitalWrite(LPWM, HIGH); // Drive left
    digitalWrite(RPWM, LOW);
  } else if (gyroX1 < -sensitivity) {
    digitalWrite(RPWM, HIGH); // Drive right
    digitalWrite(LPWM, LOW);

  } else {
    digitalWrite(RPWM, LOW); //stop motor
    digitalWrite(LPWM, LOW);
  }
}
