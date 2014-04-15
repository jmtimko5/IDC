
#define RedLED A0
#define BlueLED A2
#define GreenLED A3
#define PT 2

void setup()
{
  Serial.begin(9600);
}

long RCtime(int pin)                         // ..returns decay time
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

void loop()
{
  long sum=0;
  analogWrite(RedLED, 255);
  delay(100);
  Serial.print("Red value:");
  Serial.print(RCtime(PT));
  Serial.println();
  delay(100);
  analogWrite(RedLED, 0);
  delay(100);
  
  analogWrite(BlueLED, 200);
  delay(100);
  Serial.print("Blue value:");
  Serial.print(RCtime(PT));
  Serial.println();
  delay(100);
  analogWrite(BlueLED, 0);
  delay(100);
  
  analogWrite(GreenLED, 255);
  delay(100);
  Serial.print("Green value:");
  Serial.print(RCtime(PT));
  Serial.println();
  delay(100);
  analogWrite(GreenLED, 0);
  delay(100);
  
  delay(100);
  Serial.print("Nothing value:");
  Serial.print(RCtime(PT));
  Serial.println();
  delay(100);
}
