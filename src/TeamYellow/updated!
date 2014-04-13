/*Four Sensors Digital Inputs should be 4-7, 
Right to Left From the POV of the Bot 
stop at positionNum + 3
*/
//const int LCD = 10;
#include <Servo.h>
#include <SoftwareSerial.h> 

boolean battery = false;

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define Rx 11 // DOUT to pin 11 
#define Tx 8 // DIN to pin 8 moved from 10

SoftwareSerial Xbee (Rx, Tx); 
//SoftwareSerial mySerial = SoftwareSerial(255, LCD);

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 50;
int positionNum = 0;
int bulb = 1;
int calibLight = 80;
boolean allBlackHit = false;
int counter = 0;
int led = 9;

int whiteCount = 0;
int blackCount = 0;
int whiteLastTime = 0;
int blackLastTime = 0;
int whiteOverrideTime = 600;
int blackOverrideTime = 250;
int allWhiteCount = 0;
int allBlackCount = 0;


boolean bulbOn()
{
  return (abs(analogRead(A0) - analogRead(A1)) > calibLight);
}
void setup() 
{
  Move(0,0);
 // mySerial.begin(9600);
  
  //delay(100);
  //mySerial.write(12);                 // Clear
 // delay(5);
  //mySerial.print("Yellow");           // First line
  delay(500);
  
  
  leftServo.attach(13);               //attach servos
  rightServo.attach(12);
  rightServo.writeMicroseconds(1500);
  leftServo.writeMicroseconds(1500);
  pinMode(7, INPUT);
  //pinMode(LCD, OUTPUT);
 // digitalWrite(LCD, HIGH);
  pinMode(led, OUTPUT);
  delay(1000);
}

void loop() 
{
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;
  
  lineFollow(-1, 1);
  positionNum = positionNum + (bulb)*(int) bulbOn();
  bulb = bulb*2;
  lineFollow(-1, 1);
  //take reading
  positionNum = positionNum + (bulb)*(int) bulbOn();
  bulb = bulb*2;
  lineFollow(-1, 1);
  counter++;
  positionNum = positionNum + (bulb)*(int) bulbOn();
  
  lineFollow(4, -1);
  for (int i=0;i<400;i++) 
  {
      Move(.1,3);
      delay(1); 
  }
  
  lineFollow(-1,(6-positionNum));
  
    Move(0,0);
    for(int i=0; i=positionNum; i++)
    {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
      delay(1000);
    }
    delay(10000000);

}


// Follow lines until we've reached the specified number of stops. -1 to ignore a kind of stop
void lineFollow(int whiteStops, int blackStops) {
  blackCount = 0;
  whiteCount = 0;
  while (((whiteCount < whiteStops) || (whiteStops == -1)) && ((blackCount < blackStops) || (blackStops == -1))) {
    int irl = RCtime(IRL) > calibDiff;
    int irlc = RCtime(IRLC) > calibDiff;
    int irrc = RCtime(IRRC) > calibDiff;
    int irr = RCtime(IRR) > calibDiff;  
    //Serial.println(String(whiteCount)+" "+String(blackCount));
    //displayClear();
    //displayLCD(String(whiteCount));
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
     
  }
}

void Move(float left, float right) 
{
 float leftSpeed = mapfloat(left,0,1,1500,1700);
 float rightSpeed = mapfloat(right,0,1,1500,1350);
 
 leftServo.writeMicroseconds((int) leftSpeed);
 rightServo.writeMicroseconds((int) rightSpeed);
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long volts(int adPin)
{
  return (float(analogRead(adPin)) * 5.0/1024);
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

void resetColorCount() {
  allWhiteCount = 0;
  allBlackCount = 0;
}

void onWhite() {
  int i;
  allWhiteCount++;
  if (allWhiteCount > 5) {
    if ((millis() - whiteLastTime) > whiteOverrideTime) {
      whiteCount++;
      whiteLastTime = millis();
    }
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
}


