
// Switch Config: 
// 

#define LIM_SW_NO_PIN     8

bool isLimitPressed()
{
  static unsigned long lastTime = 0;
  static int lastReading = HIGH;      // last *raw* reading
  static int debouncedState = HIGH;   // debounced state
  const int debounce_time = 10; // ms

  int reading = digitalRead(LIM_SW_NO_PIN);

  // If the raw reading changed, reset the timer
  if (reading != lastReading)
  {
    lastTime = millis();
    lastReading = reading;
  }

  // If the reading has been stable for > debounce_time, accept it
  if (millis() - lastTime > debounce_time)
  {
    debouncedState = lastReading;
  }

  return (debouncedState == LOW);  // LOW = pressed with INPUT_PULLUP
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LIM_SW_NO_PIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (isLimitPressed() == true)
  {
    Serial.print("Switch pressed\n");
  }
  else
  {
    Serial.print("Closed\n");
  }



  delay(500);
}
