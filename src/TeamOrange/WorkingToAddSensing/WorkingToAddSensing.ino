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
#define RedLED A0
#define BlueLED A2
#define GreenLED A3
#include <Servo.h>

Servo leftServo;
Servo rightServo; //define servos
int calibDiff=50;
boolean flag=false;
int CODE=0;
int test=3;
int hashCount=0;

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

long RCtimeColour(int pin)                         // ..returns decay time
{                                            
  pinMode(pin, OUTPUT);                      // Charge capacitor
  digitalWrite(pin, HIGH);                   // ..by setting pin ouput-high
  delay(10);                                 // ..for 10 ms
  pinMode(pin, INPUT);                       // Set pin to input
  digitalWrite(pin, LOW);                    // ..with no pullup
  long time  = micros();                     // Mark the time
  while(digitalRead(pin));                   // Wait for voltage < threshold
 
  time = micros() - time;                    // Calculate decay time
 
  pinMode(pin, OUTPUT);                      // Discharge capacitor
  digitalWrite(pin, LOW);                    // ...by setting pin output-low
 
  return time;                               // Return decay time
}

long redRC()
{
  analogWrite(RedLED, 255);
  delay(1000);
  long redrc=RCtimeColour(PT);
 // Serial.print("Red value:");
 // Serial.print(redrc);
 // Serial.println();
  delay(1000);
  analogWrite(RedLED, 0);
  delay(1000);
  
  return(redrc);
}

long blueRC()
{
  analogWrite(BlueLED, 255);
  delay(1000);
  long bluerc=RCtimeColour(PT);
//  Serial.print("blue value:");
 // Serial.print(bluerc);
 // Serial.println();
  delay(1000);
  analogWrite(BlueLED, 0);
  delay(1000);
  
  return(bluerc);
}

long greenRC()
{
  analogWrite(GreenLED, 255);
  delay(1000);
  long greenrc=RCtimeColour(PT);
 // Serial.print("green value:");
 // Serial.print(greenrc);
 // Serial.println();
  delay(1000);
  analogWrite(GreenLED, 0);
  delay(1000);
  
  return(greenrc);
}

long getInteger()
{
   if (blueRC()<4000)
  {
    Serial.println("White");
    return 1;
  } else if (redRC()>25000) {
    Serial.println("Green");
    return 3;
  } else if (greenRC()<15000) {
    Serial.println("Yellow");
    return 2;
  } else {
    Serial.println("Red");
    return 0;
  }
}

void loop() 
{
  //Serial.println(RCtime(IRR));
  int irl=RCtime(IRL) > calibDiff;
  int irlc=RCtime(IRLC) > calibDiff;
  int irrc=RCtime(IRRC) > calibDiff;
  int irr=RCtime(IRR) > calibDiff;
  
  if (irl && irlc && irrc && irr) {
     // All Black
     Move(0,0);
     //tone(Speak, 440, 500);             //Tone so we know it works
     delay(500);               
     if (hashCount<3)
     {                 //if still in colour measuring part 
     CODE=CODE+getInteger();         //add measured value to CODE
     hashCount++;
     //Serial.println(hashCount);
     }
     else if(hashCount==3)
     {
       Serial.println(CODE);
       hashCount++;
     } else
     {
       if ((5-CODE)>0) 
       {
       CODE++;
       } 
       else
       {
         while(0==0){
       digitalWrite(9, HIGH);
       delay(1000);
       digitalWrite(9, LOW);
         }
       }
     }
     Move(1,1);
     delay(250); //so that it doesn't start measuring again when it's still on the black
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
