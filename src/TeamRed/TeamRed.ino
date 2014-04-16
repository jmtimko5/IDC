/*
Line Following Base Code with Four Sensors
Digital Inputs should be 4-7, Right to Left
From the POV of the Bot
//pin 9 broken

//TODO:
//-averaging values
//communication and when to go
*/

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define led 10
#define xbee 9
#include <Servo.h>
#include <SoftwareSerial.h>

Servo leftServo; //define servos
Servo rightServo;

boolean battery = true; 

 
#define Rx 11 // DOUT to pin 11
#define Tx 2 // DIN to pin 10
SoftwareSerial Xbee (Rx, Tx);

//CALIBRATION
int calibDiff = 25; //higher for darker
int senseDiff = 250;
int timeout = 5000; //(ticks)

boolean senseTrigger = false;
boolean verbose = false;
boolean check = true;
boolean check2 = true;
boolean check3 = true;

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
  
  //xbee power control
  
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  
   // XBee setup: 
   Xbee.begin(9600);
   delay(1000);
 
  
  Serial.begin(9600);
}


void Move(int left, int right) {
  delay(0);
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
  //mySerial.write(12); //clear
  //mySerial.print("Moving...");
    
  int irl = RCtime(IRL) > calibDiff;
  int irlc = RCtime(IRLC) > calibDiff;
  int irrc = RCtime(IRRC) > calibDiff;
  int irr = RCtime(IRR) > calibDiff;

  //check if done
  if (lineCount == 5 && check) {
      Serial.print("Done: ");
      check = false;
      
   } else if (lineCount == 6 && check2) {
     Serial.println("stop! staging area.");
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
     
      Move(0,0);
      Move(-1,-1);
      delay(200);
      Move(0,0);

      leftServo.detach(); //attach servos
      rightServo.detach();
      
      //display the shit
      delay(500);
      for (int i = 0; i < val; i++) {
           digitalWrite(led, HIGH);
           delay(200);
           digitalWrite(led, LOW);
           delay(100);
      }
      
     waitForSignal(val);
          
     leftServo.attach(13); //attach servos
     rightServo.attach(12);
    
      //hard coded turn
      delay(200);
      Move(1,0);
      delay(200);
      Move(1,1);
      delay(200);
                 
     //wait for change! change to false to go.
     check2 = false;
   
   } else if (lineCount == 6 + 1 && check3) {
          Serial.println("first hash mark! send moving..");
          Move(0,0);
          //turn xbee on
          delay(10);
          imGoing(val);
          //turn xbee off
          check3 = false;
   } else if (lineCount == 6 + (6 - val)) {
          Serial.println("final  area reached.");

     //stop! final area!
     Move(0,0);
     leftServo.detach(); //attach servos
     rightServo.detach();
     //done...
     
   } else {
    //hash mark code!
    //Serial.println("normal routine");
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
         //Serial.print("senseTrigger true: ");
        
         //set the sensed result in the array at key linecount
         //TODO: add value averaging
         int value = sense();
         //Serial.print(value);
         
         if (value < senseDiff) {
            //we sensed white!
            vals[lineCount] = 1;
             //Serial.println(" = WHITE");
         } else {
            //we sensed black!
            vals[lineCount] = 0;
             //Serial.println(" = BLACK");
         }
      }
      
      //debug sensor
      //Serial.println(sense());
    }  else if (!irl && !irlc && !irrc && !irr) {
      //All White
      Move(0,1);
      delay((battery) ? 3 : 3);
      Move(1,1);
      delay((battery) ? 4 : 2);
      if (verbose) {
        Serial.println("All white");
      }
    } else if (!irl && irlc && irrc && !irr) {
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


void imGoing(int pos) {
   char keymap[] =  "yuiop";
   
   for(int i= 0; i < 10; i++) {
     Xbee.print(keymap[pos]);
     delay(10);
   }
}

//This function will wait until your bot recieves a message to go. (The bot in front of you in line should 
//send this mesage.) There is a timeout that corresponds to your bot number. There is a grand timeout of 45s.
//If you send -1, your bot will go at 45 seconds.
void waitForSignal(int pos) {
   int now = millis();
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
      
      Serial.println(receiving);

      if (receiving == theirKey) {
       //my turn
       return; 
      }
     
  
    }
    
    
    //unknown value? send -1 and wait for 45. TODO: deduce position
    if (pos == -1) {
       delay(45000); //30 seconds before going
    }
    
    //time out
    if (millis() > (15000 + pos*5000)) {
        return;
    }
    
    //grand timeout
    if (millis() > 45000) {
       return; 
    }
  }
}
