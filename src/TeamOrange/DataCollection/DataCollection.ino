
#define RedLED A0
#define YelLED A3
#define GreenLED A2
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
  delay(2000);
  for (int i=0; i<50; i++)
  {
    sum=sum+RCtime(PT);
    delay(10);
  }
  Serial.print("Red value:");
  Serial.print(RCtime(PT)/50);
  Serial.println();
  delay(10000);
  analogWrite(RedLED, 0);
  delay(1000);
  
  analogWrite(YelLED, 255);
  delay(2000);
  for (int i=0; i<50; i++)
  {
    sum=sum+RCtime(PT);
    delay(10);
  }
  Serial.print("Yellow value:");
  Serial.print(RCtime(PT)/50);
  Serial.println();
  delay(10000);
  analogWrite(YelLED, 0);
  delay(1000);
  
  analogWrite(GreenLED, 255);
  delay(2000);
  for (int i=0; i<50; i++)
  {
    sum=sum+RCtime(PT);
    delay(10);
  }
  Serial.print("Green value:");
  Serial.print(RCtime(PT)/50);
  Serial.println();
  delay(10000);
  analogWrite(GreenLED, 0);
  delay(1000);
}
