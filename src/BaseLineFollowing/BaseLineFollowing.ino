/*
Line Following Base Code with Four Sensors
Digital Inputs should be 4-7, Right to Left
From the POV of the Bot
*/

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#include <Servo.h>

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 50;

void setup() 
{
  Serial.begin(9600);
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.write(90); //set to no movement
  rightServo.write(90);
}

void Move(int left, int right) {
 if (left == 1) {
 leftServo.writeMicroseconds(1700);
 } else if (left == 0) {
   leftServo.writeMicroseconds(1500);
 }
 if (right == 1) {
 rightServo.writeMicroseconds(1350);
 } else if (right == 0) {
   rightServo.writeMicroseconds(1500);
}
  
long RCtime(int sensPin){
   long result = 0;
   pinMode(sensPin, OUTPUT);       // make pin OUTPUT
   digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
   delay(1);                       // wait a  ms to make sure cap is discharged

   pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
   digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
   while(digitalRead(sensPin)){    // wait for pin to go low
      result++;
   }

   return result;                   // report results   
} 

void loop() {
  Serial.println(RCtime(IRR));
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;
  
  if (irl && irlc && irrc && irr) {
    // All Black
    Move(1,1);
  } else if (irlc && irrc) {
    // Insides Black
    Move(1,1);
  } else if (!irr && !irrc) {
    // Two Right Sides white
    Move(0,1);
  } else if (!irl && !irlc) {
    // Two Left Sides white
    Move(1,0);
  }
}
