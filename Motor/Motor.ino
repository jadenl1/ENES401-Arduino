#include "CytronMotorDriver.h" // Motor driver library, ensure it's installed

const int Rinput = A5; // Right input pin from IMU
const int Linput = A4; // Left input pin from IMU
const int speedinput = A3; // Speed input pin from IMU
const int potpin = A0; // Potentiometer pin
const int buttonStart = 8;  // Button to start the process
const int buttonIncrease = 9;  // Button to increase duration
const int buttonDecrease = 10; // Button to decrease duration

CytronMD motor(PWM_DIR, 3, 4);  // Motor pins: PWM = Pin 3, DIR = Pin 4

const int maxPWM = 175; // Max motor speed value (0-255)
int duration = 0;       // Duration in minutes


void setup() {
  Serial.begin(115200); // Begin serial communication for debugging

  // Initialize the pins
   pinMode(Linput, INPUT);
  pinMode(Rinput, INPUT);
  pinMode(speedinput, INPUT);
  pinMode(potpin, INPUT);
  pinMode(buttonStart, INPUT_PULLUP);
  pinMode(buttonIncrease, INPUT_PULLUP);
  pinMode(buttonDecrease, INPUT_PULLUP);

  Serial.println("System ready.");
}

void loop() {
  motor.setSpeed(0);
 
  // Step 1: Wait for the user to press the start button
  Serial.println("Start? Press the Start button.");
  while (digitalRead(buttonStart) == HIGH) {
    delay(100); // Poll every 100ms
  }

  // Step 2: Ask for duration input
  Serial.println("Set duration (in minutes):");


  while (true) {
    if (digitalRead(buttonIncrease) == LOW) {
      duration++;
      Serial.print("Duration: ");
      Serial.print(duration);
      Serial.println(" minutes");
      delay(300); // Debounce delay
    }
    if (digitalRead(buttonDecrease) == LOW) {
      if (duration > 0) {
        duration--;
        Serial.print("Duration: ");
        Serial.print(duration);
        Serial.println(" minutes");
      }
      delay(300); // Debounce delay
    }


    if (digitalRead(buttonStart) == LOW) {
      // Confirm duration and break out of the input loop
      Serial.print("Duration set to ");
      Serial.print(duration);
      Serial.println(" minutes.");
      break;
    }


    delay(100); // Poll every 100ms
  }

  // Step 2.5: Ask user if they want a kickstart
  Serial.println("Kickstart swing? Press + (Increase) for Yes, - (Decrease) for No.");
  bool kickstartEnabled = false;

  while (true) {
    if (digitalRead(buttonIncrease) == LOW) { // User pressed +
      kickstartEnabled = true;
      Serial.println("Kickstart enabled.");
      delay(300);
      break;
    }
    if (digitalRead(buttonDecrease) == LOW) { // User pressed -
      kickstartEnabled = false;
      Serial.println("Kickstart skipped.");
      delay(300);
      break;
    }
  }



  // Step 3: 5-second delay before starting the main loop
  Serial.println("Starting in 5 seconds...");
  delay(5000);


  // Step 4: Run the loop for the specified duration
  unsigned long startTime = millis();
  unsigned long runTime = duration * 60 * 1000; // Convert minutes to milliseconds


  while (millis() - startTime < runTime) {
    if (kickstartEnabled) {
      kickstart(); // Call the kickstart function if user chose Yes
    }
      
    swing();
    Serial.println("Swinging");


  }

  // Step 5: Ask the user to start again
  Serial.println("Time completed. Start again?");
  delay(1000); // Small delay before prompting again
}

void swing() {

    int Rreading = digitalRead(Rinput); //read the IMPU input values from ther other arduino
    int Lreading = digitalRead(Linput);
    int Spdreading = analogRead(speedinput) ;
    int Potreading = analogRead(potpint); //read potentiometer
    
    int speed = map(Potreading, 0, 1023, 0, maxPWM); //convert the potentiometer readings to the speed range of 0-255
    int taper = map(Spdreading, 0,1023, 0, 100);
    
    Serial.println(Rreading);
    Serial.println(Lreading);

    if (Lreading == HIGH) {
      if(Spdreading < speedcutoff) {
          Serial.println("left_taper")
          motor.setSpeed(taper);
      } else {
          Serial.println("left");
          motor.setSpeed(speed);
      }
     } else if (Rreading == HIGH) {
         if(Spdreading < speedcutoff) {
            Serial.println("right_taper")
            motor.setSpeed(-taper);
          } else {
              Serial.println("right");
              motor.setSpeed(-speed);
          }
      } else {
       motor.setSpeed(0);
       Serial.println("still");
     }
     // Delay for readability
     delay(100);
}

void kickstart() {
  Serial.println("Applying kickstart...");

  // Strong push in one direction
  motor.setSpeed(maxPWM);
  delay(500);

  // Reverse briefly to stabilize
  motor.setSpeed(-maxPWM);
  delay(500);

  // Stop to allow natural swing motion
  motor.setSpeed(0);
  delay(200);

  Serial.println("Kickstart complete.");
}
