/*
Hashing out method to determine integer from 1-5 from RCtime
*/

#define RedLED A0
#define YelLED A2
#define GreenLED A3
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
  long sum=0;         // will be used to find average
  long colVal=0;      // will be used to keep track of highest val
  int CODEadd=0;
  
  digitalWrite(RedLED, HIGH);    //shine red light
  delay(500);                    //delay to keep things clean
  if (RCtime(PT)>colVal) {       //compare RCtime to previous RCtime which is now zero
  colVal=RCtime(PT);             //colVal takes on the RCtime value to store temporarily
  sum=sum+colVal;
  CODEadd=0;                     //if RedLED makes the largest RCtime value then colour is red
  }
  digitalWrite(RedLED, LOW);
  delay(500);
  
  digitalWrite(YelLED, HIGH);
  delay(500);
  if (RCtime(PT)>colVal){        //compares RCtime with yelLED to redLED
  colVal=RCtime(PT);
  sum=sum+colVal;
  CODEadd=2;
  }
  digitalWrite(YelLED, LOW);
  delay(500);
  
  digitalWrite(GreenLED, HIGH);
  delay(500);
  if (RCtime(PT)>colVal){
  colVal=RCtime(PT);
  sum=sum+colVal;
  CODEadd=3;
  }
  digitalWrite(GreenLED, LOW);
  delay(500);
  
  if ((RCtime-(sum/3))<500){
  CODEadd=1;
  }
}
