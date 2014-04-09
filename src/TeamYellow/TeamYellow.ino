/*Four Sensors Digital Inputs should be 4-7, 
Right to Left From the POV of the Bot 
stop at positionNum + 3
*/
const int LCD = 10;
#include <Servo.h>
#include <SoftwareSerial.h> 


#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define Rx 11 // DOUT to pin 11 
#define Tx 8 // DIN to pin 8 moved from 10

SoftwareSerial Xbee (Rx, Tx); 
SoftwareSerial mySerial = SoftwareSerial(255, LCD);

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 50;
int positionNum = 0;
int bulb = 1;
int calibLight = 80;
boolean allBlackHit = false;
int counter = 0;
int led = 9;

void setup() 
{
  Move(0,0);
  mySerial.begin(9600);
  
  delay(100);
  mySerial.write(12);                 // Clear
  delay(5);
  mySerial.print("Yellow");           // First line
  delay(500);
  
  
  leftServo.attach(13);               //attach servos
  rightServo.attach(12);
  rightServo.writeMicroseconds(1500);
  leftServo.writeMicroseconds(1500);
  pinMode(7, INPUT);
  pinMode(LCD, OUTPUT);
  digitalWrite(LCD, HIGH);
  pinMode(led, OUTPUT);
  delay(1000);
}

void loop() 
{
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;
  
  if(!irl || !irlc || !irrc || !irr)
  {
    allBlackHit = false;
  }
  //FINISH LINE REACHED
//  if(counter == 4)
//  {
//    Move(0,0);
//    mySerial.write(12);                 // Clear             
//    mySerial.print("Position Reached");           // First line
//    mySerial.write(13);                 // Form feed
//    mySerial.print(positionNum);                 // Second line
//    
//    for(int i=0; i<positionNum; i++)
//    {
//      digitalWrite(led, HIGH);
//      delay(250);
//      digitalWrite(led, LOW);
//      delay(250);
//    }
//    delay(10000000);
//  }
  if(counter == ((6-positionNum) + 4))
  {
    Move(0,0);
    delay(5);                           // Required delay
    mySerial.print(positionNum);                 // Second line
    for(int i=0; i<positionNum; i++)
    {
      digitalWrite(led, HIGH);
      delay(250);
      digitalWrite(led, LOW);
      delay(250);
    }
    delay(10000000);
  }
  else if (!allBlackHit && irl && irlc && irrc && irr) // All Black - hash mark
  {
    allBlackHit = true;
    positionNum = positionNum + (bulb)*(int) bulbOn();
    counter++;
    bulb = bulb*2;
    if (bulb > 4)
    {
      bulb = 0;
    }
    //Serial.print((int) bulbOn());
 
    if(bulbOn())
    {
      mySerial.write(12);                 // Clear
      mySerial.print(positionNum);   // Second line
    }
    else if(!bulbOn())
    {
      mySerial.write(12);                 // Clear             
      mySerial.print(positionNum);   // Second line
    }
    Move(0,0);
    delay(10);
    Move(1,1);
    delay(400);
  }
  else if(!irlc && !irrc && !irl && !irr) // All White
  { 
    for(int i = 0; i < 2; i++) //let i go to 3 on plug power 2 on batt
    {
    Move(0, 1);
    delay(1);
    }
    for(int i = 0; i < 5; i++)
    {
    Move(1, 1);
    delay(1);
    }
  }
  else if (irlc && irrc) // Insides Black 
  { 
    Move(1,1);
  } 
  else if (!irr && !irrc) // Two Right Sides white 
  {
    Move(0,1);
  } 
  else if (!irl && !irlc) // Two Left Sides white
  {
    Move(1,0);
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

boolean bulbOn()
{
  return (abs(analogRead(A0) - analogRead(A1)) > calibLight);
}
