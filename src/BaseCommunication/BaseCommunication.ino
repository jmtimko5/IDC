/*
 * PIN SETUP
 * P13: DOUT
 * P12: DIN
 */

#include <SoftwareSerial.h> 
 
#define Rx 13 // DOUT to pin 13 
#define Tx 12 // DIN to pin 12
SoftwareSerial Xbee (Rx, Tx); 
 
void setup() { 
 Serial.begin(9600); // Set to No line ending; 
 Xbee.begin(9600); // type a char, then hit enter 
 delay(500); 
} 
 
void loop() { 
 if(Serial.available()) { // Is serial data available? 
 char outgoing = Serial.read(); // Read character, send to XBee 
 Xbee.print(outgoing); 
 } 
 
 if(Xbee.available()) { // Is data available from XBee? 
 char incoming = Xbee.read(); // Read character, 
 Serial.println(incoming); // send to Serial Monitor 
 } 
 
 delay(50); 
} 

