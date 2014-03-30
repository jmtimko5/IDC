/*
 * Adapted from Robotics with the BOE Shield - LeftLightSensor
 * 
 * Measures RC decay time for photoresistor circuit
 */
 
void setup()                                 // Built-in initialization block
{
  Serial.begin(9600);                        // Set data rate to 9600 bps
}  
 
void loop()                                  // Main loop auto-repeats
{
  long decayTime = rcTime(2);                // Uses rcTime function to calculate delay
  int sum=0;
  for (int i=0; i<50; i++)
  {
    sum=sum+rcTime(2);
  }
 
  Serial.print("Decay time = ");             // Display time label
  Serial.print(sum/50);                   // Display time value
  Serial.println(" us");                    // Display time units
 
  delay(10);                                 // 10 ms delay
}
                                             // rcTime function at pin  
long rcTime(int pin)                         // ..returns decay time
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

