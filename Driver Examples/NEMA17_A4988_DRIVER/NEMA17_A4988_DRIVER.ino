#define STEP_PIN      29
#define DIR_PIN       30
#define EN_PIN        28


#define STEPPER_ENABLE  LOW
#define STEPPER_DISABLE HIGH


void enable_stepper()
{
  digitalWrite(EN_PIN, STEPPER_ENABLE);
}

void disable_stepper()
{
  digitalWrite(EN_PIN, STEPPER_DISABLE);
}


void setup() {
  // put your setup code here, to run once:

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  digitalWrite(DIR_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  enable_stepper();

}

void loop() {
  // put your main code here, to run repeatedly:


for (int i = 0; i < 1; i++)
{
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(800);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(800);
}
}
