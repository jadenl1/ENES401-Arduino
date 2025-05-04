#include <Servo.h>

const int Rinput          = A2;
const int Linput          = A1;
const int servoPin = 9;

const int midPoint = 90;

Servo myservo;

void setup() {
    Serial.begin(115200);

      pinMode(Linput, INPUT);
      pinMode(Rinput, INPUT);

      pinMode(servoPin, INPUT);

      myservo.attach(servoPin);

      if (myservo.attached()) Serial.println("Servo success.");
      else Serial.println("Servo not connected.");
      
      myservo.write(90);
}

void loop() {
  swing(); 
}

void swing() {
  int Rreading  = digitalRead(Rinput);
  int Lreading  = digitalRead(Linput);

  Serial.print(Rreading);   // first value
Serial.print('|');        // separator
Serial.println(Lreading); // second value + newline

  int angle = 90;
//
//  myservo.write(0);
//  myservo.write(180);

//        int temp = analogRead(A5);

//      Serial.println(temp);

  if (Lreading == HIGH){
//    Serial.println("LEFT");
     myservo.write(midPoint - angle);
  } else if (Rreading == HIGH){
//    Serial.println("RIGHT");
    myservo.write(midPoint + angle);
  } else {
//    Serial.println("none");
    myservo.write(midPoint);
  }
}
