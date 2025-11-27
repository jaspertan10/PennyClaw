#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

#include "Arduino.h"

typedef enum {
  STEPPER_X_AXIS,
  STEPPER_Y_AXIS,
  STEPPER_Z_AXIS
} stepper_id_t;


typedef struct {
  int en_pin;
  int step_pin;
  int dir_pin;
  bool inverted_dir;

  void init();

  void enable();
  void disable();
  void forward();
  void backward();
  void steps(uint16_t steps, uint16_t us_delay = 800);

} stepper_driver_t;



#endif