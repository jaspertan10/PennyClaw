#define JOYSTICK_DOWN_PIN           49
#define JOYSTICK_UP_PIN             47
#define JOYSTICK_LEFT_PIN           53
#define JOYSTICK_RIGHT_PIN          51

#define ARCADE_UP_BUTTON_PIN        41
#define ARCADE_GRAB_BUTTON_PIN      43
#define ARCADE_DOWN_BUTTON_PIN      45


bool arcade_up_button_pressed()
{
    if (digitalRead(ARCADE_UP_BUTTON_PIN) == LOW)
  {
    return true;
  }

  return false;
}
bool arcade_down_button_pressed()
{
    if (digitalRead(ARCADE_DOWN_BUTTON_PIN) == LOW)
  {
    return true;
  }

  return false;
}
bool arcade_grab_button_pressed()
{
    if (digitalRead(ARCADE_GRAB_BUTTON_PIN) == LOW)
  {
    return true;
  }

  return false;
}

bool joystick_is_down_pressed()
{
  if (digitalRead(JOYSTICK_DOWN_PIN) == LOW)
  {
    return true;
  }

  return false;
}

bool joystick_is_up_pressed()
{
  if (digitalRead(JOYSTICK_UP_PIN) == LOW)
  {
    return true;
  }

  return false;
}

bool joystick_is_left_pressed()
{
  if (digitalRead(JOYSTICK_LEFT_PIN) == LOW)
  {
    return true;
  }

  return false;
}

bool joystick_is_right_pressed()
{
  if (digitalRead(JOYSTICK_RIGHT_PIN) == LOW)
  {
    return true;
  }

  return false;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(JOYSTICK_DOWN_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_UP_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_LEFT_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_RIGHT_PIN, INPUT_PULLUP);

  pinMode(ARCADE_UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ARCADE_DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ARCADE_GRAB_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (joystick_is_left_pressed())
  {
    Serial.println("Left");
  }
  if (joystick_is_right_pressed())
  {
    Serial.println("Right");
  }
  if (joystick_is_up_pressed())
  {
    Serial.println("Up");
  }
  if (joystick_is_down_pressed())
  {
    Serial.println("Down");
  }


  if (arcade_up_button_pressed())
  {
    Serial.println("Button Up");
  }
  if (arcade_down_button_pressed())
  {
    Serial.println("Button Down");
  }
  if (arcade_grab_button_pressed())
  {
    Serial.println("Button Grab");
  }
}










