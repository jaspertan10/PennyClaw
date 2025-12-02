#include "pinouts.h"
#include "stepper_driver.h"
#include "arcade_buttons.h"
#include "Servo.h"
#include <TimerOne.h>
#include <SoftwareSerial.h>

/*************** Serial LCD ****************/
SoftwareSerial OpenLCD(LCD_RX_PIN, LCD_TX_PIN); //RX, TX
byte contrast = 2; //Lower is more contrast. Contrast range = 0...5
void clear_lcd_screen();

/*************** Timer During Play Mode ****************/
#define PLAY_TIME_IN_SECONDS  30
volatile int seconds = 0;
volatile bool play_time_over = false;
void tick();
void start_timer();
void stop_timer();


/*************** IR Break Beam LED ****************/
bool penny_detected();


/*************** Servo Motor ****************/
#if (SERVO_ENABLE != 0)
#define SERVO_CLAW_CLOSE_ANGLE  95
#define SERVO_CLAW_OPEN_ANGLE   140

Servo claw_servo;
bool claw_closed = false;
#endif // SERVO_ENABLE


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







/*************** Program State Machine ****************/
typedef enum {
  INIT_STATE,
  WAIT_PENNY_STATE,
  PLAY_STATE,
  GAME_OVER_STATE,
  ERROR_STATE
} States;

//Global Variables
States current_state = INIT_STATE;

// Function forward declarations
void handle_init_state();
void handle_wait_penny();
void handle_play_state();
void handle_end_game_state();


/***************************************************************************************************************************************************/


void setup() {

/* Initialize LCD */
  OpenLCD.begin(9600);
  //Send contrast setting
  OpenLCD.write('|'); //Put LCD into setting mode
  OpenLCD.write(24); //Send contrast command
  OpenLCD.write(contrast);

  clear_lcd_screen();
  OpenLCD.print("     PennyClaw     ");
  OpenLCD.print("                    "); // Line 2 (blank)
  OpenLCD.print("   Initializing...   ");


  //Initialize IR LED signal pin
  pinMode(IR_LED_SIGNAL_PIN, INPUT_PULLUP);

#if (SERVO_ENABLE != 0)
  claw_servo.attach(SERVO_SIGNAL_PIN);
#endif


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
      handle_wait_penny();
      current_state = PLAY_STATE;
      start_timer();
    break;

    case PLAY_STATE:
      handle_play_state();
      if (play_time_over) {
        stop_timer();
        current_state = GAME_OVER_STATE;
      }
      //current_state is updated to GAME_OVER after 60 seconds of play time
    break;

    case GAME_OVER_STATE:
      handle_end_game_state();
      current_state = WAIT_PENNY_STATE;
    break;

  }

}



/***************************************************************************************************************************************************/


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

  //Initialize Z to home position
  while (!z_limit_switch_button.is_held()) {
    steppers[STEPPER_Z_AXIS].backward();
    steppers[STEPPER_Z_AXIS].steps(50);
  }

  //Initialize all step positions for all Axis to 0
  steppers[STEPPER_X_AXIS].current_step = 0;
  steppers[STEPPER_Y_AXIS].current_step = 0;
  steppers[STEPPER_Z_AXIS].current_step = 0;

  //Initialize claw position
#if (SERVO_ENABLE != 0)
  
  if (claw_closed == true) {
    claw_servo.write(SERVO_CLAW_CLOSE_ANGLE);
  }
  else {
    claw_servo.write(SERVO_CLAW_OPEN_ANGLE);
  }
#endif //SERVO_ENABLE

}

void handle_wait_penny() {

  //Update screen to say insert penny
  clear_lcd_screen();
  OpenLCD.print("                    "); // Line 1 (blank)
  OpenLCD.print("   Insert penny -->   ");

  while (penny_detected() != true) {}

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
  else {
    // Handle grab button as a *one-shot* per press
    static bool last_grab_state = false;              // remembers previous pressed state
    bool grab_now = grab_button.is_pressed();         // debounced level

    if (grab_now && !last_grab_state) {               // LOW -> HIGH edge (first frame of press)
      Serial.println("Pressed");

      if (claw_closed == true) {
        claw_servo.write(SERVO_CLAW_OPEN_ANGLE);
        claw_closed = false;
      }
      else {
        claw_servo.write(SERVO_CLAW_CLOSE_ANGLE);
        claw_closed = true;
      }
    }

    last_grab_state = grab_now;
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

void handle_end_game_state() {

  //Game over screen
  clear_lcd_screen();
  OpenLCD.print("      Game Over      ");
  OpenLCD.print("  Thanks for playing ");


  /* 
    1. Close claw
    2. Move claw up to ~200 step
    3. Move X Y gantry to home position
    4. Open claw
  */

  // Close claw
  claw_servo.write(SERVO_CLAW_CLOSE_ANGLE);

  // Move claw up to ~200 steps
  while (steppers[STEPPER_Z_AXIS].current_step > 200) {
    steppers[STEPPER_Z_AXIS].backward();
    steppers[STEPPER_Z_AXIS].steps(50);
  }

  // Move X to home position
  while (!x_limit_switch_button.is_held()) {
    steppers[STEPPER_X_AXIS].enable();
    steppers[STEPPER_X_AXIS].backward();
    steppers[STEPPER_X_AXIS].steps(50);
    steppers[STEPPER_X_AXIS].disable();
  }

  // Move Y to home position
  while (!y_limit_switch_button.is_held()) {
    steppers[STEPPER_Y_AXIS].enable();
    steppers[STEPPER_Y_AXIS].backward();
    steppers[STEPPER_Y_AXIS].steps(50);
    steppers[STEPPER_Y_AXIS].disable();
  }

  // Open claw
  claw_servo.write(SERVO_CLAW_OPEN_ANGLE);


  //Game over screen with countdown
  for (int i = 5; i >= 0; i--) {
    clear_lcd_screen();
    OpenLCD.print("      Game Over      ");
    OpenLCD.print("  Thanks for playing ");
    OpenLCD.print("                    "); //20 spaces
    OpenLCD.print("         ");  // 9 spaces
    OpenLCD.print(i);           // i = 5 → 0 (single digit)
    OpenLCD.print("          "); // 10 spaces
  
    delay(1000); // so the user can see the countdown
  }
}





bool penny_detected() {
  if (digitalRead(IR_LED_SIGNAL_PIN) == LOW)
  {
    return true;
  }
  else
  {
    return false;
  }
}


void start_timer() {
  seconds = 0;
  play_time_over = false;
  
  //1,000,000 microseconds = 1 second
  Timer1.initialize(1000000); 
  Timer1.attachInterrupt(tick);
}

void stop_timer() {
  Timer1.detachInterrupt(); 
  seconds = 0;
  play_time_over = false;
}

void tick() {
  seconds++;

  if (seconds > PLAY_TIME_IN_SECONDS) {

    play_time_over = true;
    
  }
  else {
    clear_lcd_screen();
    OpenLCD.print("     PennyClaw      ");
    if (seconds >= 10) {
      OpenLCD.print("   Time left: ");
      OpenLCD.print(seconds);
      OpenLCD.print("s   ");
    } else {
      OpenLCD.print("    Time left: ");
      OpenLCD.print(seconds);
      OpenLCD.print("s    ");
    }

    OpenLCD.print("   Get Some Candy!   ");
  }

}

void clear_lcd_screen() {
  OpenLCD.write('|');
  OpenLCD.write('-');
}











