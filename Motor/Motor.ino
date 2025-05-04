#include "CytronMotorDriver.h" // Motor driver library, ensure it's installed
#include <Wire.h>   //I2C Library
#include "LiquidCrystal_I2C.h" // Display Screen Library, ensure it's installed


const int Rinput = A2; // Right input pin from IMU
const int Linput = A1; // Left input pin from IMU
const int potpin = A0; // Potentiometer pin
const int buttonIncrease = 9;  // Button to increase duration
const int buttonDecrease = 8; // Button to decrease duration
const int buttonStart = 7; // Button to decrease duration


CytronMD motor(PWM_DIR, 3, 4);  // Motor pins: PWM = Pin 3, DIR = Pin 4

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//MODIFYING VARIABLES
const int maxPWM = 175; // Max motor speed value (0-255)
//const int speedcutoff = 5; // taper voltage cutoff
const int maxTime = 60; //maximum time in minutes to ride swing




void setup() {
  Serial.begin(115200); // Begin serial communication for debugging

//ensure display screen is connected
 if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }

    lcd.init();        // initialize the lcd 
    lcd.backlight(); //turn on lcd backlight

  // Initialize the pins
  pinMode(Linput, INPUT);
  pinMode(Rinput, INPUT);
  pinMode(potpin, INPUT);
  pinMode(buttonIncrease, INPUT_PULLUP);
  pinMode(buttonDecrease, INPUT_PULLUP);
  pinMode(buttonStart, INPUT_PULLUP);

  lcd.setCursor(0,0); //put cursor of lcd at the beginning
  lcd.print("System Ready"); // print text on lcd
  delay(2000);
  lcd.clear();
}


void loop() {
 motor.setSpeed(0);
 Serial.println("start section completed");

  // Step 1: Wait for the user to press the start button
  lcd.setCursor(0,0);
  lcd.print("SMILE SWING"); //print on the lcd
  lcd.setCursor(0,1);
  lcd.print("Press Start"); 
  while (digitalRead(buttonStart) == HIGH) { //wait for start button pressed
    delay(100); // Poll every 100ms
  }

  lcd.clear(); //clear the lcd screen

  // Step 2: Ask for duration input
  lcd.setCursor(0,0);
  lcd.print("Set Time +/- (m)");
  lcd.setCursor(0,1);
  lcd.print("Then Press Start");
while (digitalRead(buttonIncrease) == HIGH) { //wait for + button to be pressed
    delay(100); // Poll every 100ms
  }
  lcd.clear();
  int duration = 0;       // Duration in minutes

  while (true) { // loop until start button is pressed to set the time
      lcd.setCursor(0,0);
      lcd.print("Duration: ");
      lcd.setCursor(0,1);
      lcd.print(duration);
      lcd.setCursor(4,1);
      lcd.print("minutes");

    // if + button is pressed, increase time
    if (digitalRead(buttonIncrease) == LOW && duration < maxTime) { 
      duration++;
      lcd.setCursor(0,1);
      lcd.print(duration);
      delay(100); // Debounce delay
    }
    // if - button is pressed, decrease time
     if (digitalRead(buttonDecrease) == LOW) {
       if (duration > 0) {
         duration--;
        lcd.setCursor(0,1);
        lcd.print("  ");
        lcd.setCursor(0,1);
        lcd.print(duration);
      }
      delay(100); // Debounce delay
    }

    //if start button in pressed, start the swing
    if (digitalRead(buttonStart) == LOW) {
      // Confirm duration and break out of the input loop
      lcd.setCursor(0,1);
      lcd.print(duration);
      break;
    }
    delay(100); // Poll every 100ms
  }


  // Step 3: 5-second delay before starting the main loop
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Starting in");
  
  //5 second delay loop
  for(int i = 5; i > 0; i--){
    lcd.setCursor(0,1);
    lcd.print(i);
    lcd.setCursor(3,1);
    lcd.print("seconds...");
    delay(1000);
  }


  // Step 4: Run the loop for the specified duration

  //time variables to to start and stop swing
unsigned long runTime = (unsigned long)duration * 60 * 1000; // Convert minutes to milliseconds
unsigned long startTime = millis();
unsigned long resetTime = 0;


lcd.clear();
  //loop and run swing until time is out
  while ((millis() - startTime ) < runTime) {
   unsigned long currTime = millis(); //current time
    unsigned long timeLeft = (((runTime - (currTime - startTime)))/60000) + 1; //calculate the time left in minutes
    lcd.setCursor(0,0);   //print the time left
    lcd.print(timeLeft);
    lcd.setCursor(3,0); 
    lcd.print("Min Remain"); 
    
      //check if the start button has been held for 3 seconds to cancel the swing
     if (digitalRead(buttonStart) == LOW) { 
        if (resetTime == 0) { // Start timing when button is first pressed
            resetTime = millis();
        } else if (currTime - resetTime > 3000) { // If held for more than 3 seconds
            break; // Exit the loop and restart setup
        }
    } else { // Reset the timer when button is released
        resetTime = 0;
    }

    swing();  


  }


  // Step 5: Once finished, ask the user to start again
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time Completed");
  lcd.setCursor(0,1);
  lcd.print("Start Again?");
  while (digitalRead(buttonStart) == HIGH) { //if start is pressed reset
    delay(100); // Poll every 100ms
  }

}



// function for the motor to swing. Reads digital signals from the IMU arduino
// whether the swing position is on the front swing or back swing. The motor will
// rotate according to this signal. The potentiometer controls the speed of the 
// swing. The maxPWM global variable is the maximum speed, and the potentiometer will
// scale from 0 to maxPWM. 
void swing() {


int Rreading = digitalRead(Rinput); //read the IMPU input values from ther other arduino
int Lreading = digitalRead(Linput);
int Potreading = 1023 - analogRead(potpin); //read potentiometer


int speed = map(Potreading, 25, 1023, 0, maxPWM); //convert the potentiometer readings to the speed range of 0-255


 if (Lreading == HIGH) { //if left reading is high, turn motor counter clockwise
      Serial.println("left");
      motor.setSpeed(speed);
      Serial.println(speed);
  
 } else if (Rreading == HIGH) { // if right reading is high, turn motor clockwise
      Serial.println("right");
      motor.setSpeed(-speed);
      Serial.println(-speed);
  }else  {
   motor.setSpeed(0); // if the IMU signals zero velocity, turn motor off
   Serial.println(speed);
 }
}

// fucntion to ensure the display screen is connected
bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}
