/*
Hashing out method to determine integer from 1-5 from RCtime
*/

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

long redRC()
{
  analogWrite(RedLED, 255);
  delay(1000);
  long redrc=RCtime(PT);
  //Serial.print("Red value:");
  //Serial.print(redrc);
  //Serial.println();
  delay(1000);
  analogWrite(RedLED, 0);
  delay(1000);
  
  return(redrc);
}

long blueRC()
{
  analogWrite(BlueLED, 255);
  delay(1000);
  long bluerc=RCtime(PT);
 // Serial.print("blue value:");
 // Serial.print(bluerc);
 // Serial.println();
  delay(1000);
  analogWrite(BlueLED, 0);
  delay(1000);
  
  return(bluerc);
}

long greenRC()
{
  analogWrite(GreenLED, 255);
  delay(1000);
  long greenrc=RCtime(PT);
 // Serial.print("green value:");
 // Serial.print(greenrc);
 // Serial.println();
  delay(1000);
  analogWrite(GreenLED, 0);
  delay(1000);
  
  return(greenrc);
}

int getInteger()
{
   if (blueRC()<4000)
  {
    Serial.println("White");
    return 1;
  } else if (redRC()>25000) {
    Serial.println("Green");
    return 3;
  } else if (greenRC()<15000) {
    Serial.println("Yellow");
    return 2;
  } else {
    Serial.println("Red");
    return 0;
  }
}

void loop()
{
  Serial.println(getInteger());
}

  
  /*digitalWrite(RedLED, HIGH);    //shine red light
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
  
  Serial.print("CODEadd");*/
