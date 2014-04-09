/*
Line Following Base Code with Four Sensors
Digital Inputs should be 4-7, Right to Left
From the POV of the Bot
//pin 9 broken

//TODO:
//-averaging values
//communication and when to go
*/

#define din 3
#define dout 2

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define led 10
#include <Servo.h>
#include <SoftwareSerial.h>


SoftwareSerial mySerial = SoftwareSerial(255, 11);

Servo leftServo; //define servos
Servo rightServo;

boolean battery = true; 

//CALIBRATION
int calibDiff = 5;
int senseDiff = 250;
int timeout = 5000; //(ticks)

boolean senseTrigger = false;
boolean verbose = false;
boolean check = true;
boolean check2 = true;


int lineCount = 0;
int vals[5] = {0, 0, 0, 0, 0};
int val = 0; //final integer value!


void setup() 
{
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.writeMicroseconds(1500); //set to no movement
  rightServo.writeMicroseconds(1500);
  
  //init led
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
 
  //lcd shiz
  //pinMode(11, OUTPUT);
  //digitalWrite(11, HIGH);
  mySerial.begin(9600);
  delay(500);                        // Wait 3 seconds
  //mySerial.write(18);                 // Turn backlight off
  
  Serial.begin(9600);
}

void Move(int left, int right) {
 if (left == 1) {
   leftServo.writeMicroseconds(1700);
 } 
 if (left == 0) {
   leftServo.writeMicroseconds(1500);
 }
 if (right == 1) {
 rightServo.writeMicroseconds(1350);
 }
 if (right == 0) {
   rightServo.writeMicroseconds(1500);
 }
}  

long RCtime(int sensPin){
   long result = 0;
   pinMode(sensPin, OUTPUT);       // make pin OUTPUT
   digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
   (1);                       // wait a  ms to make sure cap is discharged

   pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
   digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
   while(digitalRead(sensPin) && result < timeout){    // wait for pin to go low
      result++;
   }

   return result;                   // report results
} 

void loop() {
  //mySerial.write(12); //clear
  //mySerial.print("Moving...");
  
  Serial.println(RCtime(8));
  
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;

  //check if done
  if (lineCount == 5 && check) {
     Serial.print("Done: ");
      
      
      check = false;
      
   } else if (lineCount == 6 && check2) {
     //stop! staging area!
     Move(0,0);
     Serial.print("Done: ");
     Serial.print("{");
     for (int i=0; i<5; i++) {
        val += vals[i];
        Serial.print(vals[i]);
        Serial.print(" ");
        //clear the recorded value
     }

      
     displayVal();
      
     //wait for change! change to false to go.
     check2 = false;
        
   } else if (lineCount == 6 + (6 - val)) {
     //stop! final area!
     Move(0,0);
     //done...
     
   } else {
    //hash mark code!
    if (irl && irlc && irrc && irr) {
      // All Black
      Move(1,1);
      if (verbose) {
        Serial.println("All black");
      }
      
         
      //Marker: sense!
      //Set sensing to true
      senseTrigger = true;
      if (lineCount < 5) {
         Serial.print("senseTrigger true: ");
        
         //set the sensed result in the array at key linecount
         //TODO: add value averaging
         int value = sense();
         Serial.print(value);
         
         if (value < senseDiff) {
            //we sensed white!
            vals[lineCount] = 1;
             Serial.println(" = WHITE");
         } else {
            //we sensed black!
            vals[lineCount] = 0;
             Serial.println(" = BLACK");
         }
      }
      
      //debug sensor
      //Serial.println(sense());
    }  else if (!irl && !irlc && !irrc && !irr) {
      //All White
      Move(0,1);
      delay((battery) ? 2 : 3);
      Move(1,1);
      delay((battery) ? 1 : 2);
      if (verbose) {
        Serial.println("All white");
      }
    }
      else if (!irl && irlc && irrc && !irr) {
      // Insides Black, Outsides White
      Move(1,1);
      
      //Set sensing to false when back on the white
      if (senseTrigger == true) {
         senseTrigger = false;
         if (vals[lineCount] == 0) {
           digitalWrite(led, HIGH);
           delay(101);
           digitalWrite(led, LOW);
         } else {
           digitalWrite(led, HIGH);
           delay(101);
           digitalWrite(led, LOW);
           delay(101);
           digitalWrite(led, HIGH);
           delay(101);
           digitalWrite(led, LOW);
         }
         
         
         Serial.print("Done sensing: ");
         lineCount++;
         Serial.println(lineCount);
         
       }
       if (verbose) {
         Serial.println("Insides black, outsides white");
       }
       
    } 
      else if (!irrc && !irr) {
      // Two Right Sides white
      Move(0,1);
      
      if (verbose) {
        Serial.println("Two right white");
      }
    } 
     else if (!irl && !irlc) {
      // Two Left Sides white
      Move(1,0);
      if (verbose) {
        Serial.println("Two left white");
      }
    } 
   
  }
}
 
int sense() {
   return RCtime(8);// > calibDiff;
}

void displayVal() {
  //while (true) { 
    for (int i = 0; i < val; i++) {
           digitalWrite(led, HIGH);
           delay(200);
           digitalWrite(led, LOW);
           delay(200);
      }
      mySerial.print(val); 
      delay(1000);
  //}
}
