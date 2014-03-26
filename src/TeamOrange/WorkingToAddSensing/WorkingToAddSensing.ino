/*
Line Following Base Code with Four Sensors
Digital Inputs 4-7, Right to Left
From the POV of the Bot
*/

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define PT 2
#define Speak 3
#include <Servo.h>

Servo leftServo;
Servo rightServo; //define servos
int calibDiff=50;
boolean flag=false;
int CODE=0;
int test=5;

void setup() 
{
  Serial.begin(9600);
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.write(90); //set to no movement
  rightServo.write(90);
}

void Move(int left, int right)
{
  if (left ==1)
  {
    leftServo.writeMicroseconds(1700);
  }
  else if (left==0)
  {
    leftServo.writeMicroseconds(1500);
  }
  if (right==1)
  {
    rightServo.writeMicroseconds(1350);
  }
  else if (right==0)
  {
    rightServo.writeMicroseconds(1500);
  }
}

long RCtime(int sensPin) {
  long result=0;
  pinMode(sensPin, OUTPUT);      // make pin OUTPUT
  digitalWrite(sensPin, HIGH);   // make pin HIGH to discharge capacitor - study the schematic
  delay(1);                      // wait a ms to make sure cap is discharged
  
  pinMode(sensPin, INPUT);       // turn pin into an input and time till pin goes low
  digitalWrite(sensPin, LOW);    // turn pullups off - or it won't work
  while(digitalRead(sensPin)){   // wait for pin to go low
    result++;
  }
  
  return result;                  // report results
}

long colourMeasure()
{
  long sum=0;
  for (int i=0; i<50; i++)
  {
    sum=sum+RCtime(PT);
    delay(10);
  }
  long average=sum/50;
  //here we would put some sort of if tree to determine an integer value depending on colour
  return average;
}

void loop() 
{
  Serial.println(RCtime(IRR));
  int irl=RCtime(IRL) > calibDiff;
  int irlc=RCtime(IRLC) > calibDiff;
  int irrc=RCtime(IRRC) > calibDiff;
  int irr=RCtime(IRR) > calibDiff;
  int hashCount=0;
  
  if (irl && irlc && irrc && irr) {
     // All Black
     Move(0,0);
     tone(Speak, 440, 500);             //Tone so we know it works
     delay(500);               
     if (hashCount<4) {                 //if still in colour measuring part 
     //CODE=CODE+colourMeasure();         //add measured value to CODE
     hashCount++;
     }
     else
     {
       if (hashCount>0) {
       hashCount--;
       }
       else{
       while(0==0)
       {
       }
       }
     }
     Move(1,1);
     delay(500); //so that it doesn't start measuring again when it's still on the black
  }
  else if (!irl && !irlc && !irrc && !irr) {
  //All white
  Move(1,1);
  }
  else if (!irl && irlc && irrc && !irr) {
  //insides black
  Move(1,1);
  }
  else if (!irrc && !irr) {
  // two right sides white
  Move(0,1);
  }
  else if (!irl && !irlc) {
  // two left sides white
  Move(1,0);
  }

}
