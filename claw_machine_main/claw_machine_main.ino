#include "pinouts.h"
#include "stepper_driver.h"
#include "arcade_buttons.h"


/***** A4988 Stepper Driver & Nema 17 ******/
#if (STEPPER_DRIVER_ENABLE != 0)
#define NUM_STEPPERS    3

stepper_driver_t steppers[NUM_STEPPERS] = {
  {X_STEPPER_EN_PIN, X_STEPPER_STEP_PIN, X_STEPPER_DIR_PIN, false},   //STEPPER_X_AXIS
  {Y_STEPPER_EN_PIN, Y_STEPPER_STEP_PIN, Y_STEPPER_DIR_PIN, false},   //STEPPER_Y_AXIS
  {Z_STEPPER_EN_PIN, Z_STEPPER_STEP_PIN, Z_STEPPER_DIR_PIN, false}    //STEPPER_Z_AXIS
};
#endif // STEPPER_DRIVER_ENABLE

/***** Arcade Joystick & Buttons ******/
#if (ARCADE_ENABLE != 0)

arcade_button_t joystick[ARCADE_JOYSTICK_DIRECTIONS] = {
  {JOYSTICK_UP_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME}, //JOYSTICK_UP
  {JOYSTICK_DOWN_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME}, //JOYSTICK_DOWN
  {JOYSTICK_LEFT_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME}, //JOYSTICK_LEFT
  {JOYSTICK_RIGHT_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME} //JOYSTICK_RIGHT
};

arcade_button_t up_button = {ARCADE_UP_BUTTON_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME};
arcade_button_t down_button = {ARCADE_DOWN_BUTTON_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME};
arcade_button_t grab_button = {ARCADE_GRAB_BUTTON_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME};

arcade_button_t x_limit_switch_button = {X_LIMIT_SWITCH_BUTTON_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME};
arcade_button_t y_limit_switch_button = {Y_LIMIT_SWITCH_BUTTON_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME};
arcade_button_t z_limit_switch_button = {Z_LIMIT_SWITCH_BUTTON_PIN, HIGH, ARCADE_INITIAL_LAST_CHANGE, ARCADE_DEBOUNCE_TIME};
#endif //ARCADE_ENABLE



void handle_init_state();
void handle_wait_penny();
void handle_play_state();
void handle_end_game_state();

typedef enum {
  INIT_STATE,
  WAIT_PENNY_STATE,
  PLAY_STATE,
  GAME_OVER_STATE,
  ERROR_STATE
} States;

States current_state = INIT_STATE;




/***************************************************************************************************************************************************/

void setup() {

#if (STEPPER_ENABLE != 0)
  steppers[STEPPER_X_AXIS].init();
  steppers[STEPPER_Y_AXIS].init();
  steppers[STEPPER_Z_AXIS].init();

#endif //STEPPER_ENABLE

#if (ARCADE_ENABLE != 0)

  //Initialize all joystick directions
  for (int i = 0; i < ARCADE_JOYSTICK_DIRECTIONS; i++) {
    joystick[i].init();
  }

  //Initialize arcade buttons
  up_button.init();
  down_button.init();
  grab_button.init();

  //Initialize the limit switches
  x_limit_switch_button.init();
  y_limit_switch_button.init();
  z_limit_switch_button.init();
#endif //ARCADE_ENABLE


  Serial.begin(9600);
}

int step_count = 0;

void loop() {

  if (current_state == INIT_STATE) {
    handle_init_state();
  }
  else if (current_state == WAIT_PENNY_STATE) {
    handle_wait_penny();
  }
  else if (current_state == PLAY_STATE) {
    handle_play_state();
  }
  else if (current_state == GAME_OVER_STATE) {
    handle_end_game_state();
  }


}




void handle_init_state() {

  //Initiate go home state



  current_state = WAIT_PENNY_STATE;
}

void handle_wait_penny() {
  current_state = PLAY_STATE;
}

void handle_play_state() {

  if (joystick[JOYSTICK_UP].is_pressed()) {
    Serial.println("JOYSTICK Up");
    steppers[STEPPER_Y_AXIS].forward();
    steppers[STEPPER_Y_AXIS].steps(50);
  }
  if (joystick[JOYSTICK_DOWN].is_pressed()) {
    Serial.println("JOYSTICK Down");
    steppers[STEPPER_Y_AXIS].backward();
    steppers[STEPPER_Y_AXIS].steps(50);
  }
  if (joystick[JOYSTICK_LEFT].is_pressed()) {
    Serial.println("JOYSTICK Left");
    steppers[STEPPER_X_AXIS].forward();
    steppers[STEPPER_X_AXIS].steps(50);
  }
  if (joystick[JOYSTICK_RIGHT].is_pressed()) {
    Serial.println("JOYSTICK RIGHT");
    steppers[STEPPER_X_AXIS].backward();
    steppers[STEPPER_X_AXIS].steps(50);

    step_count+= 50;
    Serial.println(step_count);
  }

  if (up_button.is_pressed()) {
    Serial.println("CLAW UP");
    steppers[STEPPER_Z_AXIS].forward();
    steppers[STEPPER_X_AXIS].steps(50);
  }
  if (down_button.is_pressed()) {
    Serial.println("CLAW DOWN");
    steppers[STEPPER_Z_AXIS].backward();
    steppers[STEPPER_X_AXIS].steps(50);
  }


  if (x_limit_switch_button.is_pressed()) {
    Serial.println("X Limit");
  }
  if (y_limit_switch_button.is_pressed()) {
    Serial.println("Y Limit");
  }
  if (z_limit_switch_button.is_pressed()) {
    Serial.println("Z Limit");
  }

  delayMicroseconds(1000);
}

void handle_end_game_state() {

}































