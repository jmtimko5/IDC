/*
Line Following Base Code with Four Sensors
Digital Inputs should be 4-7, Right to Left
From the POV of the Bot
*/

#define IRR 4
#define IRRC 5
#define IRLC 6
#define IRL 7
#define LCD 3

#define BOOL1 2
#define BOOL2 8
#define BOOL3 9

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Math.h>

Servo leftServo; //define servos
Servo rightServo;
int calibDiff = 40;

int whiteCount = 0;
int blackCount = 0;
int whiteLastTime = 0;
int blackLastTime = 0;
int whiteOverrideTime = 600;
int blackOverrideTime = 350;
int allWhiteCount = 0;
int allBlackCount = 0;
int howCountBlack = 6;
long startTime = millis();

int myNumber = 0;

boolean senseMagnet;
boolean lastSense;
boolean areWeSensing = true;

byte CLK_pin = 24;
byte EN_pin = 22;
byte DIO_pin = 26;

int X_Data = 0;
int Y_Data = 0;
int angle;
int maxPlanetNumber = 0;
int maxPlanetReading = 0;
int currentPlanetReading = 0;



 
#define Rx 11 // DOUT to pin 11 
#define Tx 10 // DIN to pin 10
SoftwareSerial Xbee (Rx, Tx);

boolean orderDeclared[] = {false, false, false, false, false};
boolean orderMoving[] =  {false, false, false, false, false};
boolean orderMovingLastChecked[] = {false, false, false, false, false};
boolean imMoving = false;
boolean someoneDoesntKnow = false;
boolean debugging = true;
 
long statusTimer = 0;
long timeSinceLastMoved = 0;
long grandFallbackTimer = 0;
long communicateTimer = 0;
int myOrder = 0;
int numBots = 5;

//SoftwareSerial mySerial = SoftwareSerial(255, LCD);

void setup() 
{
  Serial.begin(9600);
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  Move(0,0);
  
   // XBee setup: 
   Xbee.begin(9600);
   delay(3000);
   
   // Binary LEDs:
   pinMode(BOOL1, OUTPUT);
   pinMode(BOOL2, OUTPUT);
   pinMode(BOOL3, OUTPUT);
   
  pinMode(EN_pin, OUTPUT);
  pinMode(CLK_pin, OUTPUT);
  pinMode(DIO_pin, INPUT);
  pinMode(9,OUTPUT);
  digitalWrite(10,HIGH);

  HM55B_Reset();
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

void displayBinary(int number) {
  if (number % 2 > 0) {
    digitalWrite(BOOL1,HIGH);
  } else {
    digitalWrite(BOOL1,LOW);
  }
  if (number % 4 > 1) {
    digitalWrite(BOOL2,HIGH);
  } else {
    digitalWrite(BOOL2,LOW);
  }
  if (number % 8 > 3) {
    digitalWrite(BOOL3,HIGH);
  } else {
    digitalWrite(BOOL3,LOW);
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

void displayMagnet() {
   senseMagnet = magnetState();
   
   if (senseMagnet && !lastSense) {
     digitalWrite(13,HIGH);
   } else if (!senseMagnet && lastSense) {
     digitalWrite(13, LOW);
   }
   lastSense = senseMagnet;  
  
}
boolean magnetState() {
  int sensorValue = analogRead(A0);
  //Serial.println(sensorValue);
  return sensorValue > 500;
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
  if (allBlackCount > howCountBlack) {
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
  

  while ((((whiteCount < whiteStops) || (whiteStops == -1)) && ((blackCount < blackStops) || (blackStops == -1))) && ((mill < 0L) || (millis()-startTime<mill))) {
    int irl = RCtime(IRL) > calibDiff;
    int irlc = RCtime(IRLC) > calibDiff;
    int irrc = RCtime(IRRC) > calibDiff;
    int irr = RCtime(IRR) > calibDiff;
 
    float velocity = (areWeSensing) ? .3 : 1;
    
    if (irl && irlc && irrc && irr) {
      // All Black
      onBlack();
      Move(velocity,velocity);
    } 
      else if (!irl && !irlc && !irrc && !irr) {
      //All White
      onWhite();
      Move(velocity,velocity);
    }
      else if (!irl && irlc && irrc && !irr) {
      // Insides Black
      resetColorCount();
      Move(velocity,velocity);
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
    if (areWeSensing) {
      if (!(magnetState())) {
        areWeSensing = false;
         if ((blackCount > 0) && (blackCount < 6)) {
           myNumber = blackCount;
           displayBinary(myNumber);
         }
       }
    }
    
  }
}

void loop() {
  currentPlanetReading = compassValue();
  delay(40);
  
  //Begin and sense
  for (int k=1;k<=5;k++) {
    lineFollow(-1,1,-1L);
    Move(1,1);
    delay(100);
    Move(0,0);
    currentPlanetReading = compassValue();
    Serial.println(currentPlanetReading);
    if (currentPlanetReading > maxPlanetReading) {
      maxPlanetReading = currentPlanetReading;
      maxPlanetNumber = k;
      debug("NEW VALUE",-100);
    }
  }
  areWeSensing = false;
  if (myNumber < 1) {
    myNumber = maxPlanetNumber;
  }
  lineFollow(-1,1,-1L);
  
  //Stop at line
  Move(0,0);
  leftServo.detach(); //attach servos
  rightServo.detach();
  displayBinary(myNumber);
  waitForSignal(myNumber);
  //delay(2000);
  displayBinary(myNumber);
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  delay(200);
  
  //Continue to White
  lineFollow(4,-1,-1);
  
  //manuall turn
  Move(.1,3);
  delay(200);
  Move(1,1);
  delay(200);
  
  //continue manually for time
  startTime = millis();
  lineFollow(-1,-1,2000L);
  Move(0,0);
  delay(300);
  
  //go to first line
  lineFollow(-1,1,-1L);
  Move(1,1);
  delay(300);
  Move(0,0);
  delay(200);
  
  
  leftServo.detach(); //attach servos
  rightServo.detach();
  imGoing(myNumber); 
  displayBinary(7);
  leftServo.attach(13); //attach servos
  rightServo.attach(12);
  delay(200);
  howCountBlack = 7;
  
  //Finish the course
  for (int j=0;j<(5-myNumber);j++) {
    lineFollow(-1,1,-1L);
    Move(1,1);
    delay(200);
    Move(0,0);
    delay(200);
  }
  leftServo.detach(); //attach servos
  rightServo.detach();
  while(1) {
    Move(0,0);
    delay(1000);
  }
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
    if (millis() > (15000L + ((long)pos)*5000L)) {
        return;
    }
    
    //grand timeout
    if (millis() > 45000L) {
       return; 
    }
  }
}

int compassValue() {
  int sum = 0;
  float average = 0;
  int runTimes = 10;
  for (int k=0;k<runTimes;k++) {
    HM55B_StartMeasurementCommand(); // necessary!!
    delay(40); // the data is 40ms later ready
    HM55B_ReadCommand(); // read data and print Status
    //Serial.print(" ");  
    X_Data = ShiftIn(11); // Field strength in X
    Y_Data = ShiftIn(11); // and Y direction
    sum += X_Data;
    //Serial.print(X_Data); // print X strength
    //Serial.print(" ");
    //Serial.print(Y_Data); // print Y strength
    //Serial.print(" ");
    digitalWrite(EN_pin, HIGH); // ok deselect chip
    //Serial.print(angle); // print angle
  }
  sum = -sum;
  average = ((float)sum / (float)runTimes);
  return ((int)average);
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




void ShiftOut(int Value, int BitsCount) {
  for(int i = BitsCount; i >= 0; i--) {
    digitalWrite(CLK_pin, LOW);
    if ((Value & 1 << i) == ( 1 << i)) {
      digitalWrite(DIO_pin, HIGH);
      //Serial.print("1");
    }
    else {
      digitalWrite(DIO_pin, LOW);
      //Serial.print("0");
    }
    digitalWrite(CLK_pin, HIGH);
    delayMicroseconds(1);
  }
//Serial.print(" ");
}

int ShiftIn(int BitsCount) {
  int ShiftIn_result;
    ShiftIn_result = 0;
    pinMode(DIO_pin, INPUT);
    for(int i = BitsCount; i >= 0; i--) {
      digitalWrite(CLK_pin, HIGH);
      delayMicroseconds(1);
      if (digitalRead(DIO_pin) == HIGH) {
        ShiftIn_result = (ShiftIn_result << 1) + 1; 
        //Serial.print("x");
      }
      else {
        ShiftIn_result = (ShiftIn_result << 1) + 0;
        //Serial.print("_");
      }
      digitalWrite(CLK_pin, LOW);
      delayMicroseconds(1);
    }
  //Serial.print(":");

// below is difficult to understand:
// if bit 11 is Set the value is negative
// the representation of negative values you
// have to add B11111000 in the upper Byte of
// the integer.
// see: http://en.wikipedia.org/wiki/Two%27s_complement
  if ((ShiftIn_result & 1 << 11) == 1 << 11) {
    ShiftIn_result = (B11111000 << 8) | ShiftIn_result; 
  }


  return ShiftIn_result;
}

void HM55B_Reset() {
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  ShiftOut(B0000, 3);
  digitalWrite(EN_pin, HIGH);
}

void HM55B_StartMeasurementCommand() {
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  ShiftOut(B1000, 3);
  digitalWrite(EN_pin, HIGH);
}

int HM55B_ReadCommand() {
  int result = 0;
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  ShiftOut(B1100, 3);
  result = ShiftIn(3);
  return result;
}








 
