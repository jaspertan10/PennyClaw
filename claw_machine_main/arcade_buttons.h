#ifndef ARCADE_BUTTONS_H
#define ARCADE_BUTTONS_H

#include "Arduino.h"

#define ARCADE_JOYSTICK_DIRECTIONS    4
#define ARCADE_DEBOUNCE_TIME          20
#define ARCADE_INITIAL_LAST_CHANGE    0

typedef enum {
  JOYSTICK_UP,
  JOYSTICK_DOWN,
  JOYSTICK_LEFT,
  JOYSTICK_RIGHT
} joystick_directions_t;

typedef struct {

  uint8_t pin;
  bool last_state;
  unsigned long last_change_ms;
  unsigned long debounce_ms;

  void init();
  bool is_pressed();
  bool is_held();

} arcade_button_t;





#endif