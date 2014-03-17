/*
 * PIN SETUP
 * P11: DOUT
 * P10: DIN
 * P9: LED Indicator
 * P7: Button Input
 */


#include <SoftwareSerial.h> 
 
#define Rx 11 // DOUT to pin 11 
#define Tx 10 // DIN to pin 10
SoftwareSerial Xbee (Rx, Tx); 
//Change this to your squadron
String squadron = "Red";
 
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
    //Compose Message
    String message = squadron + " Squadron Reporting In";
    
    //Send Message
    Xbee.print(message);
    
    //Send Message over Serial
    Serial.print("Sending: \"");
    Serial.print(message);
    Serial.println("\"");
 }
 
 if(Xbee.available())
 {
    //Read Message
    char receiving = Xbee.read();
    
    //Indicate Message Recieved using LED
    digitalWrite(9, HIGH);
    
    //Indicate Message Recieved over Serial
    Serial.print("Receiving: \"");
    Serial.print(receiving);
    Serial.println("\"");
    
    //Blink LED off
    delay(10);
    digitalWrite(9, LOW);
 }
} 

