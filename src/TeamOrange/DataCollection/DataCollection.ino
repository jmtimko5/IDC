
#define RedLED A0
#define YelLED A3
#define GreenLED A2
#define PT 2

void setup()
{
  Serial.begin(9600);
}

long RCtime(int sensPin) {
  long result=0;
  pinMode(sensPin, OUTPUT);      // make pin OUTPUT
  digitalWrite(sensPin, HIGH);   // make pin HIGH to discharge capacitor - study the schematic
  delay(1);                      // wait a ms to make sure cap is discharged
  
  pinMode(sensPin, INPUT);       // turn pin into an input and time till pin goes low
  digitalWrite(sensPin, LOW);    // turn pullups off - or it won't work
  while(digitalRead(sensPin)){   // wait for pin to go low
    result++;
  }
  
  return result;                  // report results
}

void loop()
{
  long sum=0;
  analogWrite(RedLED, 500);
  delay(2000);
  for (int i=0; i<50; i++)
  {
    sum=sum+RCtime(PT);
  }
  Serial.print("Red value:");
  Serial.print(RCtime(PT)/50);
  Serial.println();
  delay(10000);
  analogWrite(RedLED, 0);
  delay(1000);
  
  analogWrite(YelLED, 500);
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
  
  analogWrite(GreenLED, 500);
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
