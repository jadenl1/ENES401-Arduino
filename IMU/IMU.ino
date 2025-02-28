#include <Wire.h>
#include "MPU6050.h"

int RPWM = 3;   // digital output to motor
int LPWM = 10;
int speedpin = 5; // analog output to motor

float sensitivity = 5.0;
float Vcutoff = 15.0;

// Filter configuration
float alpha = 0.1;           // Smoothing factor, adjust between 0 and 1
float filteredGyroX = 0.0;   // Initial filtered value

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize the MPU6050
  mpu.initialize();
  pinMode(LPWM, OUTPUT);
  pinMode(RPWM, OUTPUT);
  pinMode(speedpin, OUTPUT);

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
  float rawGyroX = gx / 131.0;  

  // Apply low-pass filter to smooth the data
  filteredGyroX = alpha * rawGyroX + (1 - alpha) * filteredGyroX;
  float speed = abs(filteredGyroX);

  // Print the filtered value and speed in CSV format for plotting
  Serial.print(filteredGyroX);
  Serial.print(",");
  Serial.println(speed);

  // Output speed to the other Arduino
  analogWrite(speedpin, speed);

  // Handle PWM tapering and direction based on filteredGyroX
  if (filteredGyroX > sensitivity) {
    if (filteredGyroX < Vcutoff) {
      digitalWrite(LPWM, HIGH);  // Drive left
      digitalWrite(RPWM, LOW);
    }
  } else if (filteredGyroX < -sensitivity) {
    digitalWrite(RPWM, HIGH);    // Drive right
    digitalWrite(LPWM, LOW);
  } else {
    digitalWrite(RPWM, LOW);     // Stop motor
    digitalWrite(LPWM, LOW);
  }
}
