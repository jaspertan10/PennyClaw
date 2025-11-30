#include "pinouts.h"
#include "stepper_driver.h"
#include "arcade_buttons.h"


/*************** A4988 Stepper Driver & Nema 17 ****************/
#if (STEPPER_DRIVER_ENABLE != 0)
#define NUM_STEPPERS    3
#define STEPPER_INITIAL_STEP 0

#define X_STEPPER_STEP_LIMIT    700
#define Y_STEPPER_STEP_LIMIT    900
#define Z_STEPPER_STEP_LIMIT    2000

stepper_driver_t steppers[NUM_STEPPERS] = {
  {X_STEPPER_EN_PIN, X_STEPPER_STEP_PIN, X_STEPPER_DIR_PIN, true, X_STEPPER_STEP_LIMIT, STEPPER_INITIAL_STEP},   //STEPPER_X_AXIS
  {Y_STEPPER_EN_PIN, Y_STEPPER_STEP_PIN, Y_STEPPER_DIR_PIN, false, Y_STEPPER_STEP_LIMIT, STEPPER_INITIAL_STEP},   //STEPPER_Y_AXIS
  {Z_STEPPER_EN_PIN, Z_STEPPER_STEP_PIN, Z_STEPPER_DIR_PIN, true, Z_STEPPER_STEP_LIMIT, STEPPER_INITIAL_STEP}    //STEPPER_Z_AXIS
};
#endif // STEPPER_DRIVER_ENABLE


/*************** Arcade Joystick & Buttons ****************/
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






typedef enum {
  INIT_STATE,
  WAIT_PENNY_STATE,
  PLAY_STATE,
  GAME_OVER_STATE,
  ERROR_STATE
} States;

States current_state = INIT_STATE;

void handle_init_state();
void handle_wait_penny();
void handle_play_state();
void handle_end_game_state();


/***************************************************************************************************************************************************/


void setup() {
#if (STEPPER_DRIVER_ENABLE != 0)
  steppers[STEPPER_X_AXIS].step_init();
  steppers[STEPPER_Y_AXIS].step_init();
  steppers[STEPPER_Z_AXIS].step_init();

  //Stepper Z Axis i.e. claw vertical movement stepper should always be on. This is so candy picked up will always be suspended in place. Else claw will fall repeatedly.
  steppers[STEPPER_Z_AXIS].enable();

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





void loop() {


  switch (current_state) {
    case INIT_STATE:
      handle_init_state();
      current_state = WAIT_PENNY_STATE;
    break;

    case WAIT_PENNY_STATE:
      current_state = PLAY_STATE;
    break;

    case PLAY_STATE:
      handle_play_state();
    break;

    case GAME_OVER_STATE:

    break;

  }

}




void handle_init_state() {

  //Initialize X to home position
  while (!x_limit_switch_button.is_held()) {
    steppers[STEPPER_X_AXIS].enable();
    steppers[STEPPER_X_AXIS].backward();
    steppers[STEPPER_X_AXIS].steps(50);
    steppers[STEPPER_X_AXIS].disable();
  }

  //Initialize Y to home position
  while (!y_limit_switch_button.is_held()) {
    steppers[STEPPER_Y_AXIS].enable();
    steppers[STEPPER_Y_AXIS].backward();
    steppers[STEPPER_Y_AXIS].steps(50);
    steppers[STEPPER_Y_AXIS].disable();
  }

  while (!z_limit_switch_button.is_held()) {
    steppers[STEPPER_Z_AXIS].backward();
    steppers[STEPPER_Z_AXIS].steps(50);
  }

  steppers[STEPPER_X_AXIS].current_step = 0;
  steppers[STEPPER_Y_AXIS].current_step = 0;
  steppers[STEPPER_Z_AXIS].current_step = 0;
}

void handle_wait_penny() {

}

void handle_play_state() {
  if (joystick[JOYSTICK_UP].is_pressed()) {
    if (steppers[STEPPER_Y_AXIS].current_step < steppers[STEPPER_Y_AXIS].step_limit) {
      steppers[STEPPER_Y_AXIS].enable();
      steppers[STEPPER_Y_AXIS].forward();
      steppers[STEPPER_Y_AXIS].steps(50);
      steppers[STEPPER_Y_AXIS].disable();
    }
  }
  else if (joystick[JOYSTICK_DOWN].is_pressed()) {
    if (steppers[STEPPER_Y_AXIS].current_step > 0) {
      steppers[STEPPER_Y_AXIS].enable();
      steppers[STEPPER_Y_AXIS].backward();
      steppers[STEPPER_Y_AXIS].steps(50);
      steppers[STEPPER_Y_AXIS].disable();
    }
  }
  else if (joystick[JOYSTICK_LEFT].is_pressed()) {
    if (steppers[STEPPER_X_AXIS].current_step > 0) {
      steppers[STEPPER_X_AXIS].enable();
      steppers[STEPPER_X_AXIS].backward();
      steppers[STEPPER_X_AXIS].steps(50);
      steppers[STEPPER_X_AXIS].disable();
    }
  }
  else if (joystick[JOYSTICK_RIGHT].is_pressed()) {
    if (steppers[STEPPER_X_AXIS].current_step < steppers[STEPPER_X_AXIS].step_limit) {
      steppers[STEPPER_X_AXIS].enable();
      steppers[STEPPER_X_AXIS].forward();
      steppers[STEPPER_X_AXIS].steps(50);
      steppers[STEPPER_X_AXIS].disable();
    }
  }
  else if (up_button.is_pressed()) {
    
    if (steppers[STEPPER_Z_AXIS].current_step > 0) {
      steppers[STEPPER_Z_AXIS].backward();
      steppers[STEPPER_Z_AXIS].steps(50);
    }
  }
  else if (down_button.is_pressed()) {

    if (steppers[STEPPER_Z_AXIS].current_step < steppers[STEPPER_Z_AXIS].step_limit) {
      steppers[STEPPER_Z_AXIS].forward();
      steppers[STEPPER_Z_AXIS].steps(50);
    }
  }



  /* Limit switches activated for either XYZ switch. Set current step to 0 */
  if (x_limit_switch_button.is_held()) {
    steppers[STEPPER_X_AXIS].current_step = 0;
  }
  if (y_limit_switch_button.is_held()) {
    steppers[STEPPER_Y_AXIS].current_step = 0;
  }
  if (z_limit_switch_button.is_pressed()) {
    steppers[STEPPER_Z_AXIS].current_step = 0;
  }
}

void handle_end_game_state();











