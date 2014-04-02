/*
Line Following Base Code with Four Sensors
Digital Inputs should be 4-7, Right to Left
From the POV of the Bot
*/

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define LCD 3
#include <Servo.h>
#include <SoftwareSerial.h>

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 40;

int whiteCount = 0;
int blackCount = 0;
int whiteLastTime = 0;
int blackLastTime = 0;
int whiteOverrideTime = 400;
int blackOverrideTime = 250;
int allWhiteCount = 0;
int allBlackCount = 0;

int myNumber = 0;

boolean senseMagnet;
boolean lastSense;

SoftwareSerial mySerial = SoftwareSerial(255, LCD);

void setup() 
{
  Serial.begin(9600);
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.write(90); //set to no movement
  rightServo.write(90);
  pinMode(13,OUTPUT);
  
  // Init LCD Serial
  pinMode(LCD, OUTPUT);
  digitalWrite(LCD, HIGH);
  mySerial.begin(9600);
  delay(100);
  mySerial.write(12);                 // Clear                        
  delay(5);                           // Required delay
}


void Move(float left, float right) {
 float leftSpeed = mapfloat(left,0,1,1500,1700);
 float rightSpeed = mapfloat(right,0,1,1500,1350);
 
 leftServo.writeMicroseconds((int) leftSpeed);
 rightServo.writeMicroseconds((int) rightSpeed);
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void displayLCD(String text) {
  mySerial.print(text);
}
void displayNewline() {
  mySerial.write(13);
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

void displayMagnet() {
  
   senseMagnet = magnetState();
   
   if (senseMagnet && !lastSense) {
     digitalWrite(13,HIGH);
   } else if (!senseMagnet && lastSense) {
     digitalWrite(13, LOW);
   }
   lastSense = senseMagnet;  
  
}
boolean magnetState() {
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  return sensorValue > 500;
}
void onWhite() {
  int i;
  allWhiteCount++;
  if (allWhiteCount > 5) {
    if ((millis() - whiteLastTime) > whiteOverrideTime) {
      whiteCount++;
      whiteLastTime = millis();
      if (whiteCount > 2) {
        blackCount = -100;
      }
    }
  }
  if (whiteCount == 4) {
    for (i=0;i<100;i++) {
      Move(.1,3);
      delay(1); 
    }
    blackCount = -100;
  }
}
void onBlack() {
  int i;
  allBlackCount++;
  if (allBlackCount > 3) {
    if ((millis() - blackLastTime) > blackOverrideTime) {
      blackCount++;
      blackLastTime = millis();
    }
  }
  if (blackCount == 6) {
   Move(0,0);
   delay(1500);
   whiteCount = 0;
  }
  if (blackCount == (-94-myNumber)) {
   Move(0,0);
   while(1) {
     delay(1000);
   }
  }
}
void resetColorCount() {
  allWhiteCount = 0;
  allBlackCount = 0;
}
  

void loop() {
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;
  
  
  if (irl && irlc && irrc && irr) {
    // All Black
    onBlack();
    Move(1,1);
  } 
    else if (!irl && !irlc && !irrc && !irr) {
    //All White
    onWhite();
    Move(1,1);
  }
    else if (!irl && irlc && irrc && !irr) {
    // Insides Black
    resetColorCount();
    Move(1,1);
  } 
    else if (!irrc && !irr) {
    // Two Right Sides white
    Move(0,1);
    resetColorCount();
  } 
   else if (!irl && !irlc) {
    // Two Left Sides white
    Move(1,0);
    resetColorCount();
  }
   //displayMagnet();
   if (!(magnetState())) {
     if ((blackCount > 0) && (blackCount < 6)) {
       myNumber = blackCount;
       displayLCD((String) myNumber);
     }
   }
  
}







 
