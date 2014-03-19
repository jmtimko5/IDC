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

void setup() 
{
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.write(90); //set to no movement
  rightServo.write(90);
}

void Forward(){
rightServo.write(0); //right full forward
leftServo.write(180); //left full forward
} 

void Back(){
rightServo.write(180); //right full backwards
leftServo.write(0); //left full backwards
}

void Left(){
rightServo.write(90);
leftServo.write(180);
}

void Right(){
rightServo.write(0);
leftServo.write(90);
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
  Forward();
  delay(1000);
  Back();
  delay(1000);
  Left();
  delay(1000);
  Right();
  delay(1000);
}
