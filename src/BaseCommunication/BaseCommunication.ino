/*
 * PIN SETUP
 * P11: DOUT
 * P10: DIN
 * P9: LED Indicator
 * P7: Button Input
 */

//ankit was here

#include <SoftwareSerial.h> 
 
#define Rx 11 // DOUT to pin 11 
#define Tx 10 // DIN to pin 10
SoftwareSerial Xbee (Rx, Tx); 
 
void setup() {
 pinMode(9, OUTPUT);
 pinMode(7, INPUT);
 Serial.begin(9600); // Set to No line ending; 
 Xbee.begin(9600); // type a char, then hit enter 
 delay(500); 
} 
 
void loop() {

 if(digitalRead(7) == HIGH)
 {
    Xbee.print('A');
 }
 
 if(Xbee.available())
 {
    char receiving = Xbee.read();
    digitalWrite(9, HIGH);
    delay(100);
    digitalWrite(9, LOW);
 }
} 

