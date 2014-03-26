/*Four Sensors Digital Inputs should be 4-7, 
Right to Left From the POV of the Bot
*/
const int TxPin = 8;
#include <Servo.h>
#include <SoftwareSerial.h> 

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define Rx 11 // DOUT to pin 11 
#define Tx 10 // DIN to pin 10

SoftwareSerial Xbee (Rx, Tx); 
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 50;
int positionNum = 0;
int bulb = 1;
int calibLight = 80;
boolean allBlackHit = false;
int counter = 0;

void setup() 
{
  Move(0,0);
  Serial.begin(9600);
  mySerial.begin(9600);
  
  tone(9, 3000, 1000);                       // Play tone for 1 second
  delay(1000);
  
  delay(100);
  mySerial.write(12);                 // Clear             
  mySerial.write(17);                 // Turn backlight on
  delay(5);                           // Required delay
  mySerial.print("Yellow");  // First line
  mySerial.write(13);                 // Form feed
  mySerial.print("");   // Second line
  mySerial.write(212);                // Quarter note
  mySerial.write(220);                // A tone
  
  
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  //leftServo.write(90); //set to no movement
  //rightServo.write(90);
  pinMode(7, INPUT);
  pinMode(TxPin, OUTPUT);
  digitalWrite(TxPin, HIGH);
  delay(1000);
}

void Move(int left, int right) 
{
 if (left == 1) 
 {
   leftServo.writeMicroseconds(1700);
 } 
 else if (left == 0) 
 {
   leftServo.writeMicroseconds(1500);
 }
 if (right == 1) 
 {
   rightServo.writeMicroseconds(1350);
 } 
 else if (right == 0) 
 {
   rightServo.writeMicroseconds(1500);
 }
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

void loop() 
{
  //Serial.println(RCtime(IRR));
  //Serial.print("Light Read = ");
  //Serial.print(analogRead(A0));
  //Serial.print("Ambient Read = ");
  //Serial.print(analogRead(A1));
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;
  
  if(!irl || !irlc || !irrc || !irr)
  {
    allBlackHit = false;
  }
  
  if (!allBlackHit && irl && irlc && irrc && irr) // All Black - hash mark
  {
    allBlackHit = true;
    positionNum = positionNum + (bulb)*(int) bulbOn();
    counter++;
    bulb = bulb*2;
    if (bulb > 4)
    {
      bulb = 0;
    }
    Serial.print((int) bulbOn());
 
    if(bulbOn())
    {
      mySerial.write(12);                 // Clear             
      mySerial.write(17);                 // Turn backlight on
      delay(5);                           // Required delay
      mySerial.print("Bulb Read: ON");  // First line
      mySerial.write(13);                 // Form feed
      mySerial.print(positionNum);   // Second line
      mySerial.write(212);                // Quarter note
      mySerial.write(220);                // A tone
    }
    else if(!bulbOn())
    {
      mySerial.write(12);                 // Clear             
      mySerial.write(17);                 // Turn backlight on
      delay(5);                           // Required delay
      mySerial.print("Bulb Read: OFF");  // First line
      mySerial.write(13);                 // Form feed
      mySerial.print(positionNum);   // Second line
      mySerial.write(212);                // Quarter note
      mySerial.write(220);                // A tone
    }
    Move(0,0);
    delay(10);
    Move(1,1);
    delay(400);
    
    //eventually add  pause for first three and read light state here
  }
  else if(!irlc && !irrc && !irl & !irr) // All White
  { 
    Move(1, 1);
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
