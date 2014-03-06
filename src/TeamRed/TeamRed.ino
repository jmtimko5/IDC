/*
 * PIN SETUP
 * P13: DOUT
 * P12: DIN
 */

#include <Servo.h>                           // Include servo library
 
Servo servoRight;                            // Declare right servo
Servo servoLeft;                            // Declare right servo
int serialData = 0;


void setup()                                 // Built-in initialization block
{
  servoRight.attach(12);                     // Attach right signal to pin 12
  servoLeft.attach(13);                     // Attach right signal to pin 12
  Serial.begin(9600);  
  halt();
}  
 
void loop()                                  // Main loop auto-repeats
{
  if (Serial.available() > 0)
  {
    serialData = Serial.read();
    Serial.println(serialData);
    switch (serialData) {
      case 113:
        leftForward();
        break;
     case 97:
        leftBackward();
        break;
     case 119:
        rightForward();
        break;
     case 115:
        rightBackward();
        break;
     case 49:
       halt();
       break;
      
    }

  } 
}


void halt() {
  servoRight.writeMicroseconds(1500);        // 1.5 ms stay still signal
  servoLeft.writeMicroseconds(1500);         // 1.5 ms stay still signal
}

void leftForward() {
  servoLeft.writeMicroseconds(1700);         // 1.5 ms stay still signal
}

void rightForward() {
  servoRight.writeMicroseconds(1300);         // 1.5 ms stay still signal //tuned down
}

void leftBackward() {
  servoLeft.writeMicroseconds(1300);         // 1.5 ms stay still signal
}

void rightBackward() {
  servoRight.writeMicroseconds(1700);         // 1.5 ms stay still signal
}
