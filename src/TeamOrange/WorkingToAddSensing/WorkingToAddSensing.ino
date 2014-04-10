/*
Line Following Base Code with Four Sensors
Digital Inputs 4-7, Right to Left
From the POV of the Bot
*/

const int TxPin = 10;

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define PT 2
#define RedLED A0
#define BlueLED A2
#define GreenLED A3
#include <Servo.h>
#include <SoftwareSerial.h>
#include <ParallaxLCD.h>
SoftwareSerial mySerial = SoftwareSerial(255, TxPin);


Servo leftServo;
Servo rightServo; //define servos
int calibDiff=50;
int CODE=0;
int hashCount=0;

void setup() 
{
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.write(90); //set to no movement
  rightServo.write(90);
  
  pinMode(TxPin, OUTPUT);
  digitalWrite(TxPin, HIGH); //set up LCD monitor
  
  mySerial.begin(9600);
  delay(100);
}

void loop() 
{
  //Serial.println(RCtime(IRR));
  int irl=RCtime(IRL) > calibDiff;        //check whether each QTI sees white or black
  int irlc=RCtime(IRLC) > calibDiff;
  int irrc=RCtime(IRRC) > calibDiff;
  int irr=RCtime(IRR) > calibDiff;
  
  if (irl && irlc && irrc && irr) {
     // All Black
     Move(0,0);
     delay(500);               
     if (hashCount<3)
     {                               //if still in colour measuring part 
     CODE=CODE+getInteger();         //add measured value to CODE
     hashCount++;                    //increase hashCount
     }
     else if(hashCount==3)           //if at the long hash
     {   
       mySerial.write(12);
       mySerial.print(CODE);
       hashCount++;                  //write integer value to LCD and increase hashCount
     } else
     {
       if ((5-CODE)>0)               //Count down hashes to stop at correct place
       {
       CODE++;
       } 
       else
       {
         while(0==0){                //when at correct place enter infinite loop
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
  analogWrite(RedLED, 255);                  //Turn on red LED
  delay(500);
  long redrc=RCtimeColour(PT);               //take rc time with red on
  delay(500);
  analogWrite(RedLED, 0);
  delay(500);
  
  return(redrc);
}

long blueRC()
{
  analogWrite(BlueLED, 255);                //turn on blue LED
  delay(500);
  long bluerc=RCtimeColour(PT);             //take rc time with blue on
  delay(500);
  analogWrite(BlueLED, 0);
  delay(500);
  
  return(bluerc);
}

long greenRC()
{
  analogWrite(GreenLED, 255);              //turn on green LED
  delay(500);
  long greenrc=RCtimeColour(PT);           //take rc time with green on
  delay(500);
  analogWrite(GreenLED, 0);
  delay(500);
  
  return(greenrc);
}

long getInteger()                      //method to return the colour's integer based on predetermined rc time parameters
{
   if (blueRC()<5000)
  {
    mySerial.write(12);
    mySerial.print("White");
    mySerial.write(13);
    mySerial.print("1");
    return 1;
  } else if (redRC()>25000) {
    mySerial.write(12);
    mySerial.print("Green");
    mySerial.write(13);
    mySerial.print("3");
    return 3;
  } else if (greenRC()<18000) {
    mySerial.write(12);
    mySerial.print("Yellow");
    mySerial.write(13);
    mySerial.print("2");
    return 2;
  } else {
    mySerial.write(12);
    mySerial.print("Red");
    mySerial.write(13);
    mySerial.print("0");
    return 0;
  }
}
