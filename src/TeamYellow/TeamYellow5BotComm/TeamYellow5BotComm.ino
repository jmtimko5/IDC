/*Four Sensors Digital Inputs should be 4-7, 
Right to Left From the POV of the Bot 
stop at positionNum + 3
*/
#include <Servo.h>
#include <SoftwareSerial.h> 

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define Rx 11                                               // DOUT to pin 11 
#define Tx 8                                                // DIN to pin 8 moved from 10

SoftwareSerial Xbee (Rx, Tx); 

boolean orderDeclared[] = {false, false, false, false, false};
boolean orderMoving[] =  {false, false, false, false, false};
boolean orderMovingLastChecked[] = {false, false, false, false, false};
boolean imMoving = false;
boolean someoneDoesntKnow = false;
boolean debugging = false;
boolean communicating = true;
 
long statusTimer = 0;
long timeSinceLastMoved = 0;
long grandFallbackTimer = 0;
long communicateTimer = 0;
long startTime = 0;
int myOrder = 0;
int numBots = 5;

Servo leftServo;
Servo rightServo;
int calibDiff = 50;
int positionNum = 0;
int bulb = 1;
int calibLight = 80;
boolean allBlackHit = false;
int counter = 0;
int ledred = 2;
int ledyellow = 9;
int ledgreen = 10;

int whiteCount = 0;
int blackCount = 0;
int whiteLastTime = 0;
int blackLastTime = 0;
int whiteOverrideTime = 600;
int blackOverrideTime = 250;
int allWhiteCount = 0;
int allBlackCount = 0;

void setup() 
{
  Serial.begin(9600);                                     //start computer serial
  Xbee.begin(9600);                                       //start xbee
  sDelay(500);                                            //wait half a second and run xbee
  Move(0,0);                                              //stop
  sDelay(500);                                            //wait half a second and run xbee
  
  
  leftServo.attach(13);                                   //attach left servo
  rightServo.attach(12);                                  //attach right servo
  Move(0,0);                                              //stop
  
  pinMode(7, INPUT);                                      //define input pins
  
  pinMode(ledred, OUTPUT);                                //define LED pins
  pinMode(ledyellow, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  
  digitalWrite(ledred, LOW);                              //write LED pins low
  digitalWrite(ledyellow, LOW);
  digitalWrite(ledgreen, LOW);
  
  sDelay(100);                                            //wait 1/10 second and run xbee
}

void loop() 
{
  
  lineFollow(-1, 1,-1L);                                  //follows line to first bulb
  positionNum = positionNum + (bulb)*(int) bulbOn();      //measures bulb state increments positionNum accordingly
  bulb = bulb*2;                                          //increases bulb binary position
  Bled(positionNum);                                      //displays positionNum read so far
  
  
  lineFollow(-1, 1,-1L);                                  //follows line to second bulb
  positionNum = positionNum + (bulb)*(int) bulbOn();      //measures bulb state increments positionNum accordingly
  bulb = bulb*2;                                          //increases bulb binary position
  Bled(positionNum);                                      //displays positionNum read so far
  
  
  lineFollow(-1, 1,-1L);                                  //follows line to final bulb
  positionNum = positionNum + (bulb)*(int) bulbOn();      //measures bulb state increments positionNum accordingly
  Bled(positionNum);                                      //displays positionNum
  
  
  lineFollow(-1, 1,-1L);                                  //follows line to main staging hash
  Bled(positionNum);                                      //displays positionNum
  
  
  Move(0,0);                                              //stops moving
  leftServo.detach();                                     //detach left servo
  rightServo.detach();                                    //detach right servo
  sDelay(500);                                            //wait while receiving xbee communication for half a second
  foundOrder(positionNum);   
   

   
  positionNum = doIGo();
  debug("DoIGo finished",-100);
  sDelay(3200);
  communicating = false;
   
  leftServo.attach(13);                                   //attach left servo
  rightServo.attach(12);                                  //attach right servo
  sDelay(200);                                            //wait and read xbee
  debug("servos attached",-100);
  
  lineFollow(4, -1, -1L);                                 //line follow until sharp final turn
  debug("finished white follow",-100);
   
  Move(.1, 3);                                            //manually correct for hard final turn
  sDelay(500);                                            //wait half a second
  Move(1,1);                                              //go straight
  sDelay(400);                                            //wait

  debug("finished correction movement",-100);
  communicating = true; 
  sendMoving();                                           //tell next bot to go
  startTime = millis();                                   //start timer
  lineFollow(-1,-1,1200L);                                //blindly line follow for 1.2 seconds

   
  debug("sending moving",-100);
  debug("beginning final",-100);
  lineFollow(-1, 6-positionNum,-1L);                      //get to final hash

  Move(0,0);                                              //stop
  leftServo.detach();                                     //detach left servo           
  rightServo.detach();                                    //detach right servo
  while(1)                                                //stop indefinitely
  {
    sDelay(1000);
    Move(0,0);
  }
}


boolean bulbOn()
{
  return (abs(analogRead(A0) - analogRead(A1)) > calibLight);
}

// Follow lines until we've reached the specified number of stops. -1 to ignore a kind of stop
void lineFollow(int whiteStops, int blackStops, long mill) 
{
  blackCount = 0;
  whiteCount = 0;
  
 
  while ((((whiteCount < whiteStops) || (whiteStops == -1)) && ((blackCount < blackStops) || (blackStops == -1))) && ((mill < 0L) || (millis()-startTime<mill))) 
  {
    int irl = RCtime(IRL) > calibDiff;
    int irlc = RCtime(IRLC) > calibDiff;
    int irrc = RCtime(IRRC) > calibDiff;
    int irr = RCtime(IRR) > calibDiff;
    
    if (irl && irlc && irrc && irr)                        // All Black
    {
    onBlack();
    Move(1,1);
    } 
    else if (!irl && !irlc && !irrc && !irr)               //All White
    {
    onWhite();
    Move(1,1);
    }
    else if (!irl && irlc && irrc && !irr)                 //Insides Black
    {
    resetColorCount();
    Move(1,1);
    } 
    else if (!irrc && !irr)                                //Two Right Sides white
    {
    Move(0,1);
    resetColorCount();
    } 
    else if (!irl && !irlc)                                //Two Left Sides white
    {
    Move(1,0);
    resetColorCount();
    }
    
  }
}


void Bled(int positionNum) {
  if(positionNum == 1)
  {
     digitalWrite(ledred, HIGH);
     digitalWrite(ledyellow, LOW);
     digitalWrite(ledgreen, LOW);
  }
  else if(positionNum == 2)
  {
     digitalWrite(ledred, LOW);
     digitalWrite(ledyellow, HIGH);
     digitalWrite(ledgreen, LOW);
  }
  else if(positionNum == 3)
  {
     digitalWrite(ledred, HIGH);
     digitalWrite(ledyellow, HIGH);
     digitalWrite(ledgreen, LOW);
  }
  else if(positionNum == 4)
  {
     digitalWrite(ledred, LOW);
     digitalWrite(ledyellow, LOW);
     digitalWrite(ledgreen, HIGH);
  }
  else if(positionNum == 5)
  {
     digitalWrite(ledred, HIGH);
     digitalWrite(ledyellow, LOW);
     digitalWrite(ledgreen, HIGH);
  }
}
  
   
void Move(float left, float right) 
{
 float leftSpeed = mapfloat(left,0,1,1500,1700);
 float rightSpeed = mapfloat(right,0,1,1500,1350);
 
 leftServo.writeMicroseconds((int) leftSpeed);
 rightServo.writeMicroseconds((int) rightSpeed);
}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


long volts(int adPin)
{
  return (float(analogRead(adPin)) * 5.0/1024);
}


long RCtime(int sensPin)
{
   long result = 0;
   pinMode(sensPin, OUTPUT);       // make pin OUTPUT
   digitalWrite(sensPin, HIGH);    // make pin HIGH to discharge capacitor - study the schematic
   sDelay(1);                       // wait a  ms to make sure cap is discharged

   pinMode(sensPin, INPUT);        // turn pin into an input and time till pin goes low
   digitalWrite(sensPin, LOW);     // turn pullups off - or it won't work
   while(digitalRead(sensPin)){    // wait for pin to go low
      result++;
   }

   return result;                   // report results   
} 

void resetColorCount() 
{
  allWhiteCount = 0;
  allBlackCount = 0;
}

void onWhite() 
{
  allWhiteCount++;
  if (allWhiteCount > 8) 
  {
    if ((millis() - whiteLastTime) > whiteOverrideTime) 
    {
      whiteCount++;
      debug("White added",-100);
      whiteLastTime = millis();
    }
  }
}

void onBlack()
{
  allBlackCount++;
  if (allBlackCount > 6) 
  {
    if ((millis() - blackLastTime) > blackOverrideTime) 
    {
      blackCount++;
      debug("Black Added",-100);
      blackLastTime = millis();
    }
  }
}







//Below is Communication

// Delay function that allows for constant communication
// Can also be used with mills = 0 for calling as often as you want
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
String checksum(String data) 
{
  int sum = 0;
  double math = 0;
  
  for (int i=0L;i<data.length();i++) 
  {
    sum += (int) ((char) data.charAt(i));
  }
  math = (double) sum;
  // The complicated mathematical hash generator. 1.8 is a good number.
  math = pow(math,2) / 10.0;
  sum = ((int) math) % 16;
  
  if (sum < 10) 
  {
    // Return String character 0-9 (ascii 48-57)
    return String(char(sum + 48));
  } 
  else 
  {
    // Return String character a-f (ascii 97-102)
    return String(char(sum + 87));
  }
}

// Overarching Send/Recieve Function
void communicate() 
{
  String buffer = "";
  String filtered = "";
  int len;
  int i;
  int readChar = 1;
  
  // Read whole buffer into String
  while (Xbee.available() && readChar <=3) 
  {
    //Read Character
    char receiving = Xbee.read();
    buffer += String(receiving);
    readChar++;
  }
  if (buffer != "") 
  {
    //debug("Buffer: "+buffer,-100);
    
    // Take away all non wanted characters. Keep a-k,G-K, 1-9, and '='
    for (i=0;i<buffer.length();i++) 
    {
      char c = buffer.charAt(i);
      if ((c>=97 && c<=107) || (c>=71 && c<=75) || (c>=48 && c<=57) || (c == 61)) 
      {
        filtered += String(c);
      }
    }
    
    // If we have the start of a 'packet'
    if (String(filtered.charAt(0)) == "=") 
    {
      String data = String(filtered.charAt(1));
      String hash = String(filtered.charAt(2));
      
      // Check for integrity of packet
      if (checksum("=" + data) == hash) 
      {
        // g-k declares an order position, G-K says you've started to go down the final stretch
        // As far as I can tell, charAt is the only way to typecast back to char
        debug("checksum confirmed: ="+data+hash,-100);
        switch (data.charAt(0)) 
        {
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
        if (myOrder == -1) 
        {
          int numFalses = 0;
          int newOrder = 0;
          for (i=0;i<numBots;i++) 
          {
            if (orderDeclared[i] == false) 
            
            {
              numFalses++;
              newOrder = i;
            }
          }
          newOrder += 1;
          if (numFalses == 1) 
          {
            debug(">>New Order Deduced",-100);
            foundOrder(newOrder);
          } 
        }
      }      
    }
  }
  
  // If it's been more than 1.5 seconds, update our status
  if ((millis() - statusTimer) > 1500L) 
  {  
    sendStatus();
    statusTimer = millis();
  }
}

// Let's us see the bot's logs on what numbers are taken
void debugOrder() 
{
  int j;
  String orderString = "Orders: ";
  for (j=0;j<numBots;j++) 
  {
    orderString = orderString + String(orderDeclared[j]);
  }
  debug(orderString,-100);
  String movingString = "Moving: ";
  for (j=0;j<numBots;j++) 
  {
    movingString = movingString + String(orderMoving[j]);
  }
  debug(movingString,-100); 
} 

// Called automatically by communicate() to broadcast our status
void sendStatus() 
{
  debugOrder();
  
  String data2 = "";
  // If we have our order number and it's unique
  if ((myOrder != 0)) 
  {
    // There's been an error and don't know my number
    if (myOrder == -1) 
    {
      data2 = String("?");
    } 
    else if (imMoving) 
    {
      // Ascii G-K
      data2 = String(char(myOrder + 70));
    } 
    else 
    {
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
int doIGo() 
{
  debug(">>At black line, waiting to go.",-100);
  timeSinceLastMoved = millis();
  while(1) 
  {
    communicate();
    sDelay(20);
    
    // Timer that resets itself every time a new bot moves
    if (arrayEqual(orderMoving,orderMovingLastChecked) == false) 
    {
      timeSinceLastMoved = millis();
      // Fuck C. orderMovingLastChecked = orderMoving;
      for (int j=0;j<numBots;j++) 
      {
        orderMovingLastChecked[j] = orderMoving[j];
      }
      debug(">>A bot has moved!",-100);
    }
    // Only set this the first time
    if (grandFallbackTimer != 0L) 
    {
      grandFallbackTimer = millis();
    }
    // Everything is going right so far
    if ((myOrder != -1) && (myOrder != 0)) 
    {
      // I am number one
      if (myOrder == 1) 
      {
        debug(">>I'm going first",-100);
        return myOrder;
      }
      // Other basic case: person in front of me has gone
      if (orderMoving[myOrder-2] == true) 
      {
        debug(">>Bot in front has gone, I'm leaving as: ",myOrder);
        return myOrder;
      }
      // They haven't gone yet, but the person ahead of them has 
      // and it's been over 45 sec. We assume that either they went mute
      // or the bot somehow totally died, so we go ahead anyways.
      if ((myOrder > 2) && (orderMoving[myOrder-3] == true) && ((millis()-timeSinceLastMoved)>45000L)) 
      {
        debug(">>Timeout for bot ahead, I'm leaving as: ", myOrder);
        return myOrder;
      }
      // Same thing, but I'm bot number two
      if ((myOrder == 2) && (millis()-timeSinceLastMoved)>45000L) 
      {
        debug(">>Bot 1 has timed out and I'm bot two. Leaving as: ",myOrder);
        return myOrder;
      }
    }
    // Grand Fallback Time: in case everything has gone wrong, we can at least do a staggered start
    long timeToWait = 0L;
    if (myOrder == -1) 
    {
      timeToWait = 120000L;
    } 
    else if (myOrder == 0) 
    {
      timeToWait = 130000L;
    } 
    else 
    {
      timeToWait = 90000L + 10000L*myOrder;
    }
    if ((millis()-grandFallbackTimer) > timeToWait) 
    {
      if ((myOrder == -1) || (myOrder == 0)) 
      {
        myOrder = numBots; //Might as well just put them last
      }
      debug(">>Grand Fallback Time Exceeded. Leaving as: ", myOrder);
      return myOrder;
    }
  }
}

// Call this when you know your number, or with -1 if you don't
void foundOrder(int orderNum) 
{
  debug(">>Found Order, requesting number ",orderNum);
  if ((orderNum <= 5) && (orderNum >= -1))
  {
    if ((orderDeclared[orderNum-1] == false) && (orderMoving[orderNum-1] == false) && (orderNum != -1)) 
    {
      myOrder = orderNum;
      orderDeclared[myOrder-1] = true;
    } 
    else
    {
      debug(">>Order Number Conflict or unsuccessful. Now -1",-100);
      myOrder = -1;
      someoneDoesntKnow = true;
    }
  } 
  else 
  {
    myOrder = -1;
    someoneDoesntKnow = true;
  }
  debug(">>Found Order finished, we are now number  ",myOrder);
  sendStatus();
}

// Yes, the length is hard coded. So don't use it in other code.
boolean arrayEqual(boolean *a, boolean *b)
{
  int n;
  int len = numBots;
  for (n=0;n<len;n++) 
  {
    if (a[n]!=b[n]) 
    {
      return false;
    }
  }
  return true;
}

// Sets state to let other bots know who has moved
void sendMoving() 
{
  imMoving = true;
  orderMoving[myOrder-1] = true;
}

// Little serial debugging function. -100 as int makes the int argument optional
void debug(String text, int number) 
{
 if (debugging) 
 {
   String message = "";
   if (number != -100) 
   {
     message = text + number;
   } 
   else 
   {
     message = text;
   }
   Serial.println(message);
 }
} 
