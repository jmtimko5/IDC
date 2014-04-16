/*
Line Following Base Code with Four Sensors
Digital Inputs should be 4-7, Right to Left
From the POV of the Bot

Updated 4/16/14
We have adhered to the Duke Community standard in completing this assignment.
Signed, 
Bridget Dou, Logan Rooper
*/

//The line sensor pins:
#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7

//Other pin constants
#define led 10
#define xbee 9
#define Rx 11 // DOUT to pin 11
#define Tx 2 // DIN to pin 10

//Libraries
#include <Servo.h>
#include <SoftwareSerial.h>

//Declare fields
Servo leftServo; //define servos
Servo rightServo;

//Declare and set fields
boolean battery = true; 
SoftwareSerial Xbee (Rx, Tx);
boolean senseTrigger = false;
boolean verbose = false;
int lineCount = 0;
int vals[5] = {0, 0, 0, 0, 0};
int val = 0; //final integer value!

//Subroutine checks
boolean check = true;
boolean check2 = true;
boolean check3 = true;

//CALIBRATION fields
int calibDiff = 5; //higher for darker ambient light settings
int senseDiff = 250;
int timeout = 5000; //(ticks)

void setup() 
{
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  leftServo.writeMicroseconds(1500); //set to no movement
  rightServo.writeMicroseconds(1500);
  
  //init led
  pinMode(led, OUTPUT);
  Serial.begin(9600);

  //light led to indicate   
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  
  // XBee setup: 
  Xbee.begin(9600);
  delay(1000);
}

//Usage: Move(1,1) to go foward. Move(1,0) to go right. Move(0,1) to go left.
//-1 makes wheel go in reverse. 
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
 if (right == -1) {
 rightServo.writeMicroseconds(1700);
 }
 if (left == -1) {
   leftServo.writeMicroseconds(1350);
 }
}  

//Measure the RCtime by looping until the qti returns high. Lower number means 'brighter' surface
long RCtime(int sensPin){
   long result = 0;
   pinMode(sensPin, OUTPUT);       // make pin OUTPUT
   digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
   delay(1);                       // wait a  ms to make sure cap is discharged

   pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
   digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
   while(digitalRead(sensPin) && result < timeout){    // wait for pin to go low
      result++;
   }
   return result;                   // report results
} 


void loop() {
  //First, our subroutines.
  
  //Check if hash count is 6 and we haven't run through this subroutine before.
  if (lineCount == 6 && check2) {
     //We've reached the staging area.
     stagingArea();
   } else if (lineCount == 7 & check3) {
     firstHashMark();
   } else if (lineCount == 6 + (6 - val)) {
     finalArea();
   } else {
    //Main routine: follow lines and count line hashes.
    //Measure 'brightness' for each line sensing QTI
    int irl = RCtime(IRL) > calibDiff;
    int irlc = RCtime(IRLC) > calibDiff;
    int irrc = RCtime(IRRC) > calibDiff;
    int irr = RCtime(IRR) > calibDiff;
    if (irl && irlc && irrc && irr) {
      // Line Sense: All Black. Must be a hash.
      Move(1,1);
      if (verbose) {
          Serial.println("All black");
       }
       
      //Set sensing to true. Now we wait until we find the end of the hash.
      senseTrigger = true;
      
      //Sense block color if this is hash 0-4.  Beyond this, we don't care about sensor readings.
      if (lineCount < 5) {        
         //Get the value from the block sensor.
         int value = sense();
         
         //Compare it to a threshold to see if it's white or black block.         
         if (value < senseDiff) {
            //we sensed white! put it into our block value storage array at the index of our hashmark.
            vals[lineCount] = 1;
         } else {
            //we sensed black! do nothing.
            vals[lineCount] = 0;
         }
      }
      
    }  else if (!irl && !irlc && !irrc && !irr) {
      //All White. This should only happen in the 'twilight zones' when the bots are merging.
      //Make a hardcoded partial left turn to counter for being unable to navigate this corner.
      Move(0,1);
      delay((battery) ? 3 : 3); //Ternary notation here: if we're not on battery we should use a different turning angle.
      Move(1,1);
      delay((battery) ? 4 : 2); //Other part of the ratio here.
      if (verbose) {
        Serial.println("All white");
      }
    } else if (!irl && irlc && irrc && !irr) {
      // Insides Black, Outsides White. This is normal line. Just follow straight.
      Move(1,1);
      
      //If we just came off of a hash, we can detect this now. Stop sensing and flash our LED to indicate which value we found.
      if (senseTrigger == true) {
         senseTrigger = false;
         if (vals[lineCount] == 0) {
           //Flash once if we found a black block
           digitalWrite(led, HIGH);
           delay(101);
           digitalWrite(led, LOW);
         } else {
           //Flash twice if we found a black block
           digitalWrite(led, HIGH);
           delay(101);
           digitalWrite(led, LOW);
           delay(101);
           digitalWrite(led, HIGH);
           delay(101);
           digitalWrite(led, LOW);
         }
         
         //Increment our line count.         
         lineCount++;
         Serial.println(lineCount);
         
       }
       if (verbose) {
         Serial.println("Insides black, outsides white");
       }
       
    } 
      else if (!irrc && !irr) {
      // Two Right Sides white. Turn left.
      Move(0,1);
      
      if (verbose) {
        Serial.println("Two right white");
      }
    } 
     else if (!irl && !irlc) {
      // Two Left Sides white. Turn right.
      Move(1,0);
      if (verbose) {
        Serial.println("Two left white");
      }
    } 
   
  }
}
 
 
 
//Usage: call to return the value of the QTI block sensor.
int sense() {
   return RCtime(8);// > calibDiff;
}


//Call this to tell the bot in position behind you that it's their turn to go.
void imGoing(int pos) {
   char keymap[] =  "yuiop";
   
   for(int i= 0; i < 10; i++) {
     Xbee.print(keymap[pos]);
     delay(10);
   }
}

//This function will wait until your bot recieves a message to go. (The bot in front of you in line should 
//send this mesage using "imGoing(val)".) There is a timeout that corresponds to your bot number. There is a grand timeout of 45s.
//If you send -1, your bot will go at 45 seconds. This indicates an error on your bot has occured.
void waitForSignal(int pos) {
   long now = millis();
   while(1) {
     //if i'm bot 1, I go
     if (pos == 1) {
       //time to go!
       return; 
     }
     
     //the values
     char keymap[] =  "yuiop";
     char theirKey = keymap[pos-1];
    
     //read xbee until the person in front of you says going
     while (Xbee.available()) {
      //Read Character
      char receiving = Xbee.read();
      //if char is the id of the bot in front of me, I should go
      if (receiving == theirKey) {
       //my turn
       return; 
      }
    }
    
    //unknown value? send -1 and wait for 45. TODO: deduce position
    if (pos == -1) {
       delay(45000L); //30 seconds before going
    }
    
    //time out
    if (millis() > (15000L + ((long)pos)*10000L)) {
        return;
    }
    
    //grand timeout
    if (millis() > 45000L) {
       return; 
    }
  }
  
  void stagingArea() {
    //Stop tbe bot, calculate the number based on our array values.
     Move(0,0);
     Serial.print("Done: ");
     Serial.print("{");
     for (int i=0; i<5; i++) {
        val += vals[i];
        Serial.print(vals[i]);
        Serial.print(" ");
        //clear the recorded value
     }
     
      //Back up to the line, we don't want to get in anyones way.
      Move(0,0);
      Move(-1,-1);
      delay(120);
      Move(0,0);
      
      //Turn off the servos so that they don't move around while we're waiting.
      leftServo.detach();
      rightServo.detach();
      
      //Display our value by flashing the LED.
      delay(500);
      for (int i = 0; i < val; i++) {
           digitalWrite(led, HIGH);
           delay(200);
           digitalWrite(led, LOW);
           delay(100);
      }
      
     //Sit on the line and wait until a bot ahead of us calls our number. Or go if we're one.
     waitForSignal(val);
     
     //Go time! Turn on servos.
     leftServo.attach(13); //attach servos
     rightServo.attach(12);
    
      //hard coded turn. This first white spot was tricky.
      delay(200);
      Move(1,0);
      delay(200);
      Move(1,1);
      delay(200);
      
      //Don't catch this subroutine again.                 
      check2 = false;
    
  }
  
  //Call this upon reaching the first hash mark.
  void firstHashMark() {
      Serial.println("first hash mark! send moving..");
      //Stop so the Xbee can send without drawing too much current
      Move(0,0);
      //turn xbee on
      delay(10);
      //Tell other bots that we're going.
      imGoing(val);
      //turn xbee off
      check3 = false; 
  }
  
  //Call this upon reaching the final area
  void finalArea() {
     //Stop and detach. We're done.
     Move(0,0);
     leftServo.detach(); //attach servos
     rightServo.detach();
     //done... 
     //No check on this subroutine. This will loop forever.
  }
}
