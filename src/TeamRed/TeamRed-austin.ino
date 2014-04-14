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
#include <Servo.h>
#include <SoftwareSerial.h>


SoftwareSerial mySerial = SoftwareSerial(255, 11);

Servo leftServo; //define servos
Servo rightServo;

boolean battery = true; 

 
#define Rx 2 // DOUT to pin 11 
#define Tx 3 // DIN to pin 10
SoftwareSerial Xbee (Rx, Tx);

//COMMUNICATION
boolean orderDeclared[] = {false, false, false};
boolean orderMoving[] =  {false, false, false};
boolean orderMovingLastChecked[] = {false, false, false};
boolean imMoving = false;
boolean someoneDoesntKnow = false;
boolean debugging = true;
 
long statusTimer = 0 ;
long timeSinceLastMoved = 0;
long grandFallbackTimer = 0;
long communicateTimer = 0;
int myOrder = 0;
int numBots = 4;

//CALIBRATION
int calibDiff = 5;
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
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  
   // XBee setup: 
 Xbee.begin(9600);
 delay(3000);
 
  //lcd shiz
  //pinMode(11, OUTPUT);
  //digitalWrite(11, HIGH);
  mySerial.begin(9600);
  delay(500);                        // Wait 3 seconds
  //mySerial.write(18);                 // Turn backlight off
  
  Serial.begin(9600);
}

void Move(int left, int right) {
  sDelay(0);
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
     int v = val;
     
     if (v != 0) {
         foundOrder(v);
     } else {
         foundOrder(-1);
     }
     
     displayVal();
     sDelay(3100);
     val = doIGo();

      
    
      
     //wait for change! change to false to go.
     check2 = false;
   
   } else if (lineCount == 6 + 1 && check3) {
       sendMoving();
       check3 = false;
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
           sDelay(101);
           digitalWrite(led, LOW);
         } else {
           digitalWrite(led, HIGH);
           sDelay(101);
           digitalWrite(led, LOW);
           sDelay(101);
           digitalWrite(led, HIGH);
           sDelay(101);
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
           delay(500);
           digitalWrite(led, HIGH);
           delay(200);
           digitalWrite(led, LOW);
           delay(200);
      }
      
      Move(-1, -1);
      //mySerial.print(val); 
      delay(200);
      Move(0,0);
      //mySerial.print(val); 
      //delay(800);
      
      
  //}
}








//whoa






// Delay function that allows for constant communication
// Can also be used with mills = 0 for calling as often as you want
void sDelay(int mills) { 
  if (millis() - communicateTimer > 10) {
   communicate();
   communicateTimer = 0L;
  } 
  for (int j=0;j < mills; j+=5) {
    delay(5);
    if (millis() - communicateTimer > 20L) {
      communicate();
      communicateTimer = 0L;
    }
  }
}

// Create a one character hexidecimal checksum (ascii a-f,0-9)
String checksum(String data) {
  int sum = 0;
  double math = 0;
  
  for (int i=0L;i<data.length();i++) {
    sum += (int) ((char) data.charAt(i));
  }
  math = (double) sum;
  // The complicated mathematical hash generator. 1.8 is a good number.
  math = pow(math,2) / 10.0;
  sum = ((int) math) % 16;
  
  if (sum < 10) {
    // Return String character 0-9 (ascii 48-57)
    return String(char(sum + 48));
  } else {
    // Return String character a-f (ascii 97-102)
    return String(char(sum + 87));
  }
}

// Overarching Send/Recieve Function
void communicate() {
  String buffer = "";
  String filtered = "";
  int len;
  int i;
  int readChar = 1;
  
  // Read whole buffer into String
  while (Xbee.available() && readChar <=3) {
    //Read Character
    char receiving = Xbee.read();
    buffer += String(receiving);
    readChar++;
  }
  if (buffer != "") {
    //debug("Buffer: "+buffer,-100);
    
    // Take away all non wanted characters. Keep a-k,G-K, 1-9, and '='
    for (i=0;i<buffer.length();i++) {
      char c = buffer.charAt(i);
      if ((c>=97 && c<=107) || (c>=71 && c<=75) || (c>=48 && c<=57) || (c == 61)) {
        filtered += String(c);
      }
    }
    
    // If we have the start of a 'packet'
    if (String(filtered.charAt(0)) == "=") {
      String data = String(filtered.charAt(1));
      String hash = String(filtered.charAt(2));
      
      // Check for integrity of packet
      if (checksum("=" + data) == hash) {
        // g-k declares an order position, G-K says you've started to go down the final stretch
        // As far as I can tell, charAt is the only way to typecast back to char
        debug("checksum confirmed: ="+data+hash,-100);
        switch (data.charAt(0)) {
          case 'g':
            orderDeclared[0] = true;
            break;
          case 'h':
            orderDeclared[1] = true;
            break;
          case 'i':
            orderDeclared[2] = true;
            break;
          case 'j':
            orderDeclared[3] = true;
            break;
          case 'k':
            orderDeclared[4] = true;
            break;
          case 'G':
            orderMoving[0] = true;
            break;
          case 'H':
            orderMoving[1] = true;
            break;
          case 'I':
            orderMoving[2] = true;
            break;
          case 'J':
            orderMoving[3] = true;
            break;
          case 'K':
            orderMoving[4] = true;
            break;
          case '?':
            someoneDoesntKnow = true;
            break;      
        }
        // if we were an unknown order but the rest have filled,  we can deduce ours
        if (myOrder == -1) {
          int numFalses = 0;
          int newOrder = 0;
          for (i=0;i<numBots;i++) {
            if (orderDeclared[i] == false) {
              numFalses++;
              newOrder = i;
            }
          }
          newOrder += 1;
          if (numFalses == 1) {
            debug(">>New Order Deduced",-100);
            foundOrder(newOrder);
          } 
        }
      }      
    }
  }
  
  // If it's been more than 1.5 seconds, update our status
  if ((millis() - statusTimer) > 1500L) {  
    sendStatus();
    statusTimer = millis();
  }
}

// Let's us see the bot's logs on what numbers are taken
void debugOrder() {
  int j;
  String orderString = "Orders: ";
  for (j=0;j<numBots;j++) {
    orderString = orderString + String(orderDeclared[j]);
  }
  debug(orderString,-100);
  String movingString = "Moving: ";
  for (j=0;j<numBots;j++) {
    movingString = movingString + String(orderMoving[j]);
  }
  debug(movingString,-100); 
} 

// Called automatically by communicate() to broadcast our status
void sendStatus() {
  debugOrder();
  
  String data2 = "";
  // If we have our order number and it's unique
  if ((myOrder != 0)) {
    // There's been an error and don't know my number
    if (myOrder == -1) {
      data2 = String("?");
    } else if (imMoving) {
      // Ascii G-K
      data2 = String(char(myOrder + 70));
    } else {
      // Ascii g-k
      data2 = String(char(myOrder + 102));
    }
    String hash = checksum("=" + data2);
    String packet = "=" + data2 + hash;
    debug("Sending: "+packet,-100);
    Xbee.print(packet);
  } 
}

void sendDoge() {
   Xbee.println("░░░░░░░░░▄░░░░░░░░░░░░░░▄");
   Xbee.println("░░░░░░░░▌▒█░░░░░░░░░░░▄▀▒▌");
   Xbee.println("░░░░░░░░▌▒▒█░░░░░░░░▄▀▒▒▒▐");
   Xbee.println("░░░░░░░▐▄▀▒▒▀▀▀▀▄▄▄▀▒▒▒▒▒▐");
   Xbee.println("░░░░░▄▄▀▒░▒▒▒▒▒▒▒▒▒█▒▒▄█▒▐");
   Xbee.println("░░░▄▀▒▒▒░░░▒▒▒░░░▒▒▒▀██▀▒▌");
   Xbee.println("░░▐▒▒▒▄▄▒▒▒▒░░░▒▒▒▒▒▒▒▀▄▒▒▌");
   Xbee.println("░░▌░░▌█▀▒▒▒▒▒▄▀█▄▒▒▒▒▒▒▒█▒▐");
   Xbee.println("░▐░░░▒▒▒▒▒▒▒▒▌██▀▒▒░░░▒▒▒▀▄▌");
   Xbee.println("░▌░▒▄██▄▒▒▒▒▒▒▒▒▒░░░░░░▒▒▒▒▌");
   Xbee.println("▌▒▀▐▄█▄█▌▄░▀▒▒░░░░░░░░░░▒▒▒▐");
   Xbee.println("▐▒▒▐▀▐▀▒░▄▄▒▄▒▒▒▒▒▒░▒░▒░▒▒▒▒▌");
   Xbee.println("▐▒▒▒▀▀▄▄▒▒▒▄▒▒▒▒▒▒▒▒░▒░▒░▒▒▐");
   Xbee.println("░▌▒▒▒▒▒▒▀▀▀▒▒▒▒▒▒░▒░▒░▒░▒▒▒▌");
   Xbee.println("░▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▒▄▒▒▐");
   Xbee.println("░░▀▄▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▄▒▒▒▒▌");
   Xbee.println("░░░░▀▄▒▒▒▒▒▒▒▒▒▒▄▄▄▀▒▒▒▒▄▀");
   Xbee.println("░░░░░░▀▄▄▄▄▄▄▀▀▀▒▒▒▒▒▄▄▀");
   Xbee.println("░░░░░░░░░▒▒▒▒▒▒▒▒▒▒▀▀");
}

// Returns 0 to wait, your (new) order number if it is time for you to go
int doIGo() {
  debug(">>At black line, waiting to go.",-100);
  timeSinceLastMoved = millis();
  while(1) {
    communicate();
    delay(20);
    
    // Timer that resets itself every time a new bot moves
    if (arrayEqual(orderMoving,orderMovingLastChecked) == false) {
      timeSinceLastMoved = millis();
      // Fuck C. orderMovingLastChecked = orderMoving;
      for (int j=0;j<numBots;j++) {
        orderMovingLastChecked[j] = orderMoving[j];
      }
      debug(">>A bot has moved!",-100);
    }
    // Only set this the first time
    if (grandFallbackTimer != 0L) {
      grandFallbackTimer = millis();
    }
    // Everything is going right so far
    if ((myOrder != -1) && (myOrder != 0)) {
      // I am number one
      if (myOrder == 1) {
        debug(">>I'm going first",-100);
        return myOrder;
      }
      // Other basic case: person in front of me has gone
      if (orderMoving[myOrder-2] == true) {
        debug(">>Bot in front has gone, I'm leaving as: ",myOrder);
        return myOrder;
      }
      // They haven't gone yet, but the person ahead of them has 
      // and it's been over 45 sec. We assume that either they went mute
      // or the bot somehow totally died, so we go ahead anyways.
      if ((myOrder > 2) && (orderMoving[myOrder-3] == true) && ((millis()-timeSinceLastMoved)>45000L)) {
        debug(">>Timeout for bot ahead, I'm leaving as: ",myOrder);
        return myOrder;
      }
      // Same thing, but I'm bot number two
      if ((myOrder == 2) && (millis()-timeSinceLastMoved)>45000L) {
        debug(">>Bot 1 has timed out and I'm bot two. Leaving as: ",myOrder);
        return myOrder;
      }
    }
    // Grand Fallback Time: in case everything has gone wrong, we can at least do a staggered start
    long timeToWait = 0L;
    if (myOrder == -1) {
      timeToWait = 120000L;
    } else if (myOrder == 0) {
      timeToWait = 130000L;
    } else {
      timeToWait = 90000L + 10000L*myOrder;
    }
    if ((millis()-grandFallbackTimer) > timeToWait) {
      if ((myOrder == -1) || (myOrder == 0)) {
        myOrder = numBots; //Might as well just put them last
      }
      debug(">>Grand Fallback Time Exceeded. Leaving as: ", myOrder);
      return myOrder;
    }
  }
}

// Call this when you know your number, or with -1 if you don't
void foundOrder(int orderNum) {
  debug(">>Found Order, requesting number ",orderNum);
  if ((orderNum <= 5) && (orderNum >= -1)) {
    if ((orderDeclared[orderNum-1] == false) && (orderMoving[orderNum-1] == false) && (orderNum != -1)) {
      myOrder = orderNum;
      orderDeclared[myOrder-1] = true;
    } else {
      debug(">>Order Number Conflict or unsuccessful. Now -1",-100);
      myOrder = -1;
      someoneDoesntKnow = true;
    }
  } else {
    myOrder = -1;
    someoneDoesntKnow = true;
  }
  debug(">>Found Order finished, we are now number  ",myOrder);
  sendStatus();
}

// Yes, the length is hard coded. So don't use it in other code.
boolean arrayEqual(boolean *a, boolean *b){
  int n;
  int len = numBots;
  for (n=0;n<len;n++) {
    if (a[n]!=b[n]) {
      return false;
    }
  }
  return true;
}

// Sets state to let other bots know who has moved
void sendMoving() {
  imMoving = true;
  orderMoving[myOrder-1] = true;
}

// Little serial debugging function. -100 as int makes the int argument optional
void debug(String text, int number) {
 if (debugging) {
   String message = "";
   if (number != -100) {
     message = text + number;
   } else {
     message = text;
   }
   Serial.println(message);
 }
} 

