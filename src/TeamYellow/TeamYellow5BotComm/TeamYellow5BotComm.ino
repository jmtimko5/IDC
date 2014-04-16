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
  delay(500);                                            //wait half a second and run xbee
  Move(0,0);                                              //stop
  delay(500);                                            //wait half a second and run xbee
  
  
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
  
  delay(100);                                            //wait 1/10 second and run xbee
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
  delay(500);                                            //wait while receiving xbee communication for half a second
  //foundOrder(positionNum);   
   

   
  //positionNum = doIGo();
  //debug("DoIGo finished",-100);
  waitForSignal(positionNum);
  delay(3200);
  //communicating = false;
   
  leftServo.attach(13);                                   //attach left servo
  rightServo.attach(12);                                  //attach right servo
  delay(200);                                            //wait and read xbee
  //debug("servos attached",-100);
  
  lineFollow(4, -1, -1L);                                 //line follow until sharp final turn
  //debug("finished white follow",-100);
   
  Move(.1, 3);                                            //manually correct for hard final turn
  delay(500);                                            //wait half a second
  Move(1,1);                                              //go straight
  delay(400);                                            //wait

  //debug("finished correction movement",-100);
  //communicating = true; 
  //sendMoving();                                         //tell next bot to go
  
  startTime = millis();                                   //start timer
  lineFollow(-1,-1,1200L);                                //blindly line follow for 1.2 seconds

   
  //debug("sending moving",-100);
  //debug("beginning final",-100);
  lineFollow(-1, ((6-positionNum)+3),-1L);                      //get to final hash
  imGoing(positionNum);

  Move(0,0);                                              //stop
  leftServo.detach();                                     //detach left servo           
  rightServo.detach();                                    //detach right servo
  while(1)                                                //stop indefinitely
  {
    delay(1000);
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
    else if(!irl && irlc && !irrc && !irr)
    {
    Move(1, .8);
    delay(200);
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
   delay(1);                       // wait a  ms to make sure cap is discharged

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
      //debug("White added",-100);
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
      //debug("Black Added",-100);
      blackLastTime = millis();
    }
  }
}



void imGoing(int pos) 
{
   char keymap[] =  "yuiop";
   for (int i = 0; i < 10; i++) {
     Xbee.print(keymap[pos]);
     delay(10);
   }
}
 
//This function will wait until your bot recieves a message to go. (The bot in front of you in line should 
//send this mesage.) There is a timeout that corresponds to your bot number. There is a grand timeout of 45s.
//If you send -1, your bot will go at 45 seconds.
void waitForSignal(int pos) 
{
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
