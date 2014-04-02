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
#define onesens 8
//#define twosens 9

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 50;
int calibsen = 400;
int counter = 0;
int contact = 2; 
int accu = 15;
int c0 = 0; 
int c1 = 100; 
int c2 = 200; 
int c3 = 300; 
int c4 = 430; 
int c5 = 1000;
int report = 0; 
int black = 0;
int tick = 0; 
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
 leftServo.writeMicroseconds(1600);
 } else if (left == 0) {
   leftServo.writeMicroseconds(1500);
 }
 if (right == 1) {
 rightServo.writeMicroseconds(1425);
 } else if (right == 0) {
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

int PegValue(int counting){
  int Peg = 0;
  if (counting >=c0 && counting <=c1){
  Peg = 1;
  }
  
  if (counting >c1 && counting <=c2){
  Peg = 2;
  }
  
  if (counting >c2 && counting <=c3){
  Peg = 3;
  }
  
  if (counting >c3 && counting <=c4){
  Peg = 4;
  }
  
  if (counting >c4 && counting <=c5){
  Peg = 5;
  }
  
  return Peg;
}
void onBlack(){
  if(black==0 && tick!=report){
    
    Move(0,0);
    tick++;
    delay(3000);
    black = 1; 
  }
  else if (black == 1){
    Move(1,1);
  }
  else if(tick == report){
  Move(0,0);
  }
}
void loop() {
  //Serial.println(RCtime(onesens));
  //Serial.println("\n");
  //Serial.println(RCtime(twosens));
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;
  int Answer;
  
  if (irl && irlc && irrc && irr) {
    // All Black
    onBlack();
    //Serial.println(tick);
  } 
  else if (irlc && irrc) {
    // Insides Black
    Move(1,1);
  } else if (!irr && !irrc) {
    // Two Right Sides white
    Move(0,1);
  } else if (!irl && !irlc) {
    // Two Left Sides white
    Move(1,0);
  }
  if (!irr && !irl){
    black = 0; 
  }
  
  
  int d1 = RCtime(onesens) > calibsen;
  //int d2 = RCtime(twosens) > calibsen;
  if (!d1 && contact==2){
    accu--;
    if (accu==0){
      contact--;
      accu=15; 
    } 
  }
  if (d1 && contact==1 && counter>=0){ 
    counter++; 
    //Serial.println(counter); 
  }
  if (!d1 && contact==1 && counter>15){ 
    accu--;
    if (accu==0){
      contact--;
      report = PegValue(counter); 
      Serial.println(report);
      Serial.println(counter);
    }  
    
  }
}
