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

void Move(float left, float right) {
 float leftSpeed = mapfloat(left,0,1,1500,1700);
 float rightSpeed = mapfloat(right,0,1,1500,1350);
 Serial.println(leftSpeed);
 
 leftServo.writeMicroseconds((int) leftSpeed);
 rightServo.writeMicroseconds((int) rightSpeed);
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
  } 
    else if (!irl && !irlc && !irrc && !irr) {
    //All White
    Move(1,1);
  }
    else if (!irl && irlc && irrc && !irr) {
    // Insides Black
    Move(1,1);
  } 
    else if (!irrc && !irr) {
    // Two Right Sides white
    Move(0,1);
  } 
   else if (!irl && !irlc) {
    // Two Left Sides white
    Move(1,0);
  } 
   
   
  }
 
