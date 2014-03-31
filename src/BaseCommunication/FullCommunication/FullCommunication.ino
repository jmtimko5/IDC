/*
 * PIN SETUP
 * P11: DOUT
 * P10: DIN
 */
#include <SoftwareSerial.h>
#include <Math.h>
 
#define Rx 11 // DOUT to pin 11 
#define Tx 10 // DIN to pin 10
SoftwareSerial Xbee (Rx, Tx);

boolean orderDeclared[] = {false, false};
boolean orderMoving[] =  {false, false};
boolean orderMovingLastChecked[] = {false, false};
boolean imMoving = false;
boolean someoneDoesntKnow = false;
 
long statusTimer = 0;
long timeSinceLastMoved = 0;
long grandFallbackTimer = 0;
long communicateTimer = 0;
int myOrder = 0;
 
void setup() {
 pinMode(9, OUTPUT);
 pinMode(7, INPUT);
 Serial.begin(9600); // Set to No line ending; 
 Xbee.begin(9600); // type a char, then hit enter
 
 delay(3000);
}
 
void loop() {
  sDelay(5000);
  foundOrder(2);
  sDelay(5000);
  int order = doIGo();
  while(1) {
    debug("I'm now finishing with Order: ",order);
    sDelay(150);
  }
  
} 

// Delay function that allows for constant communication
void sDelay(int mills) { 
  if (millis() - communicateTimer > 20) {
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
          for (i=0;i<sizeof(myOrder);i++) {
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
  
  // If it's been more than half a second, update our status
  if ((millis() - statusTimer) > 1500L) {  
    sendStatus();
    statusTimer = millis();
  }
}

void sendStatus() {
  String orderString = "Orders: "+String(orderDeclared[0])+String(orderDeclared[1])+String(orderDeclared[2])+String(orderDeclared[3])+String(orderDeclared[4]);
  debug(orderString,-100);
  String movingString = "Moving: "+String(orderMoving[0])+String(orderMoving[1])+String(orderMoving[2])+String(orderMoving[3])+String(orderMoving[4]);
  debug(movingString,-100);
  
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

// Returns 0 to wait, your (new) order number if it is time for you to go
int doIGo() {
  while(1) {
    communicate();
    delay(20);
    
    // Timer that resets itself every time a new bot moves
    if (arrayEqual(orderMoving,orderMovingLastChecked) == false) {
      timeSinceLastMoved = millis();
      // Fuck C. orderMovingLastChecked = orderMoving;
      for (int j=0;j<sizeof(orderMoving);j++) {
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
        imMoving = true;
        orderMoving[myOrder-1] = true;
        return myOrder;
      }
      // Other basic case: person in front of me has gone
      if (orderMoving[myOrder-1] == true) {
        debug(">>Bot in front has gone, I'm leaving as: ",myOrder);
        imMoving = true;
        orderMoving[myOrder-1] = true;
        return myOrder;
      }
      // They haven't gone yet, but the person ahead of them has 
      // and it's been over 30 sec. We assume that either they went mute
      // or the bot somehow totally died, so we go ahead anyways.
      if ((orderMoving[myOrder-1] == true) && ((millis()-timeSinceLastMoved)>30000L)) {
        debug(">>Timeout for bot ahead, I'm leaving as: ",myOrder);
        imMoving = true;
        orderMoving[myOrder-1] = true;
        return myOrder;
      }
    }
    // Grand Fallback Time: in case everything has gone wrong, we can at least do a staggered start
    long timeToWait = 0L;
    if (myOrder == -1) {
      timeToWait = 90000L;
    } else if (myOrder == 0) {
      timeToWait = 100000L;
    } else {
      timeToWait = 60000L + 10000L*myOrder;
    }
    if ((millis()-grandFallbackTimer) > timeToWait) {
      if ((myOrder == -1) || (myOrder == 0)) {
        myOrder = sizeof(orderDeclared); //Might as well just put them last
      }
      debug(">>Grand Fallback Time Exceeded. Leaving as: ", myOrder);
      orderMoving[myOrder-1] = true;
      return myOrder;
    }
  }
}
void foundOrder(int orderNum) {
  debug(">>Found Order, requesting number ",orderNum);
  if ((orderNum <= 5) && (orderNum >= -1)) {
    if ((orderDeclared[orderNum-1] == false) && (orderMoving[orderNum-1] == false) && (orderNum != -1)) {
      myOrder = orderNum;
      orderDeclared[myOrder-1] = true;
    } else {
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

boolean arrayEqual(boolean *a, boolean *b){
  int n;
  int len = sizeof(a);
  
  for (n=0;n<len;n++) {
    if (a[n]!=b[n]) {
      return false;
    }
  }
  return true;
}

// Little serial debugging function. -100 as int makes it optional
void debug(String text, int number) {
 String message = "";
 if (number != -100) {
   message = text + number;
 } else {
   message = text;
 }
 Serial.println(message);
} 



