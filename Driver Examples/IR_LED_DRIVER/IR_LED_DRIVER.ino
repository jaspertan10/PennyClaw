
#define SIGNAL_IR_LED_PIN   13

typedef enum
{
  BEAM_BLOCKED,
  BEAM_CLEAR
} ir_led_status_t;

ir_led_status_t get_ir_led_status()
{
  if (digitalRead(SIGNAL_IR_LED_PIN) == LOW)
  {
    return BEAM_BLOCKED;
  }
  else //SIGNAL_IR_LED_PIN == BEAM_CLEAR
  {
    return BEAM_CLEAR;
  }
}


void setup() {
  // put your setup code here, to run once:

  // ***IMPORTANT*** Ensure signal pin has an internal pullup
  pinMode(SIGNAL_IR_LED_PIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (get_ir_led_status() == BEAM_BLOCKED)
  {
    //Something blocking IR LED
    Serial.print("BLOCKED\n");
  }
  else //SIGNAL_IR_LED_PIN == BEAM_CLEAR
  {
    //Unobstructed LED
    Serial.print("CLEAR\n"); 
  }
  
  delay(500);
}
