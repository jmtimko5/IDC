/*
Line Following and Colour sensing code, working to add communication
 Digital Inputs 4-7, Right to Left
 From the POV of the Bot
 */

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define PT 2
#define RedLED A0
#define BlueLED A2
#define GreenLED A3
#define Rx 11
#define Tx 10
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Math.h>
SoftwareSerial Xbee (Rx, Tx);


Servo leftServo;
Servo rightServo; //define servos
int calibDiff=50;
int CODE=0;
int code2=0;
int hashCount=0;


int whiteCount = 0;
int blackCount = 0;
int whiteLastTime = 0;
int blackLastTime = 0;
int whiteOverrideTime = 600;
int blackOverrideTime = 350;
int allWhiteCount = 0;
int allBlackCount = 0;

long startTime = millis();

boolean orderDeclared[] = {
  false, false, false, false, false};
boolean orderMoving[] =  {
  false, false, false, false, false};
boolean orderMovingLastChecked[] = {
  false, false, false, false, false};
boolean imMoving = false;
boolean someoneDoesntKnow = false;
boolean debugging = true;
boolean communicating=true;

long statusTimer = 0;
long timeSinceLastMoved = 0;
long grandFallbackTimer = 0;
long communicateTimer = 0;
int myOrder = 0;
int numBots = 5;

void setup() 
{
  leftServo.attach(13); //attach servos
  rightServo.attach(12);

  pinMode(8,OUTPUT);

  //Serial.begin(9600);
  Xbee.begin(9600);
  delay(3000);
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
    {                //if still in colour measuring part 

      int dummy=getInteger();     //get colour value and store temporarily

      flash(dummy);                //flash value
      CODE=CODE+dummy;         //add measured value to CODE
      Serial.println(String(CODE)+" "+String(dummy));
      hashCount++;                    //increase hashCount
    }
    else if(hashCount==3) {           //if at the long hash   
      //     foundOrder(CODE);              //communicate integer that has been found
      Serial.println(CODE);
      code2=CODE;
      hashCount++;

      Move(-1,-1);                  //back up
      delay(200);
      Move(0,0);

      leftServo.detach(); //attach servos
      rightServo.detach();

      flash(CODE);                  //flash final code value
      //   CODE=doIGo();                 //communicate with other robots to determine order
      waitForSignal(CODE);
      //delay(2000);
      communicating=false;

      leftServo.attach(13); //attach servos
      rightServo.attach(12);
      delay(200);
      
      lineFollow(4,-1,-1);
      Move(0,1);
      delay(300);
      
      //time go
      lineFollow(-1,-1,2000L);
      Move(0,0);
      delay(1000);
      
      //get to first hash
      lineFollow(-1,1,-1L);
      Move(1,1);
      delay(200);
      Move(0,0);
      leftServo.detach();
      rightServo.detach();
      imGoing(CODE);
      leftServo.attach(13);
      rightServo.attach(12);
      delay(200);
      
      for (int j=0;j<(5-CODE);j++) {
        lineFollow(-1,1,-1L);
        Move(1,1);
        delay(200);
        Move(0,0);
        delay(200);
      }
      Move(0,0);
      leftServo.detach();
      rightServo.detach();
      while(1) {
        Move(0,0);
        delay(1000);
      }
      

    } 
    else if (hashCount==4) {      //extra hash b/c it backs up a bit
      hashCount++;
    }
    else {
      if ((5-CODE)>0)  {             //Count down hashes to stop at correct place
        if (hashCount==5) {
          communicating=true;
          //sendMoving();
          //Serial.println("im sending moving");
          CODE++;
        } 
        else {
          //Serial.println("im not");
          CODE++;
        }
      } 
      else {
        imGoing(code2);
        flash(1);
        delay(1000000);

        //leftServo.detach(); //attach servos
        //rightServo.detach();
        //while(1) {                //when at correct place enter infinite loop
        //flash(1);
        //}
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

void flash(int count) {
  for (int i=count; i>0; i--) {
    digitalWrite(8, HIGH);
    delay(150);
    digitalWrite(8, LOW);
    delay(100);
  }
}

void Move(float left, float right) {
  delay(0);
  float leftSpeed = mapfloat(left,0,1,1500,1700);
  float rightSpeed = mapfloat(right,0,1,1500,1350);

  leftServo.writeMicroseconds((int) leftSpeed);
  rightServo.writeMicroseconds((int) rightSpeed);
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
  delay(50);
  long redrc=RCtimeColour(PT);               //take rc time with red on
  delay(50);
  analogWrite(RedLED, 0);
  delay(50);

  return(redrc);
}

long blueRC()
{
  analogWrite(BlueLED, 255);                //turn on blue LED
  delay(50);
  long bluerc=RCtimeColour(PT);             //take rc time with blue on
  delay(50);
  analogWrite(BlueLED, 0);
  delay(50);

  return(bluerc);
}

long greenRC()
{
  analogWrite(GreenLED, 255);              //turn on green LED
  delay(50);
  long greenrc=RCtimeColour(PT);           //take rc time with green on
  delay(50);
  analogWrite(GreenLED, 0);
  delay(50);

  return(greenrc);
}

long getInteger()                      //method to return the colour's integer based on predetermined rc time parameters
{
  if (blueRC()<5000)
  {
    return 1;
  } 
  else if (redRC()>25000) {
    return 3;
  } 
  else if (greenRC()<18000) {
    return 2;
  } 
  else {
    return 0;
  }
}

void sDelay(int mills) { 
  if (millis() - communicateTimer > 20L) {
    if (communicating) {
      communicate();
    }
    communicateTimer = 0L;
  } 
  for (int j=0;j < mills; j+=5) {
    delay(5);
    if (millis() - communicateTimer > 20L) {
      if (communicating) {
        communicate();
      }
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
  } 
  else {
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
    } 
    else if (imMoving) {
      // Ascii G-K
      data2 = String(char(myOrder + 70));
    } 
    else {
      // Ascii g-k
      data2 = String(char(myOrder + 102));
    }
    String hash = checksum("=" + data2);
    String packet = "=" + data2 + hash;
    debug("Sending: "+packet,-100);
    Xbee.print(packet);
  } 
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
    } 
    else if (myOrder == 0) {
      timeToWait = 130000L;
    } 
    else {
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
    } 
    else {
      debug(">>Order Number Conflict or unsuccessful. Now -1",-100);
      myOrder = -1;
      someoneDoesntKnow = true;
    }
  } 
  else {
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
    } 
    else {
      message = text;
    }
    Serial.println(message);
  }
} 

void imGoing(int pos) {
  char keymap[] =  "yuiop";
  for (int i=0; i < 20; i++) {
    Xbee.print(keymap[pos]);
    delay(50);
  }

}
//This function will wait until your bot recieves a message to go. (The bot in front of you in line should 
//send this mesage.) There is a timeout that corresponds to your bot number. There is a grand timeout of 45s.
//If you send -1, your bot will go at 45 seconds.
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
    if (millis() > (30000L + ((long)pos)*10000L)) {
        return;
    }
    
    //grand timeout
    if (millis() > 90000L) {
       return; 
    }
  }
}



void onWhite() {
  int i;
  allWhiteCount++;
  if (allWhiteCount > 5) {
    if ((millis() - whiteLastTime) > whiteOverrideTime) {
      whiteCount++;
      debug("White Plus One",-100);
      whiteLastTime = millis();
    }
  }
}
void onBlack() {
  int i;
  allBlackCount++;
  if (allBlackCount > 6) {
    if ((millis() - blackLastTime) > blackOverrideTime) {
      blackCount++;
      debug("Black Plus One",-100);
      blackLastTime = millis();
    }
  }
}
void resetColorCount() {
  allWhiteCount = 0;
  allBlackCount = 0;
}


// Follow lines until we've reached the specified number of stops. -1 to ignore a kind of stop
void lineFollow(int whiteStops, int blackStops, long mill) {
  blackCount = 0;
  whiteCount = 0;
  startTime = millis();
  

  while ((((whiteCount < whiteStops) || (whiteStops == -1)) && ((blackCount < blackStops) || (blackStops == -1))) && ((mill < 0L) || (millis()-startTime<mill))) {
    int irl = RCtime(IRL) > calibDiff;
    int irlc = RCtime(IRLC) > calibDiff;
    int irrc = RCtime(IRRC) > calibDiff;
    int irr = RCtime(IRR) > calibDiff;
 
    
    if (irl && irlc && irrc && irr) {
      // All Black
      onBlack();
      Move(1,1);
    } 
      else if (!irl && !irlc && !irrc && !irr) {
      //All White
      onWhite();
      Move(1,1);
    }
      else if (!irl && irlc && irrc && !irr) {
      // Insides Black
      resetColorCount();
      Move(1,1);
    } 
      else if (!irrc && !irr) {
      // Two Right Sides white
      Move(0,1);
      resetColorCount();
    } 
     else if (!irl && !irlc) {
      // Two Left Sides white
      Move(1,0);
      resetColorCount();
    }
    
  }
}



