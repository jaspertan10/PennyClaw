#include "stepper_driver.h"


void stepper_driver_t::step_init() {
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(en_pin, OUTPUT);

  digitalWrite(dir_pin, inverted_dir ? LOW : HIGH);
  digitalWrite(step_pin, LOW);
  disable();
}

void stepper_driver_t::enable() {
  digitalWrite(en_pin, LOW);
}

void stepper_driver_t::disable() {
  digitalWrite(en_pin, HIGH);
}

void stepper_driver_t::forward() {
  digitalWrite(dir_pin, inverted_dir ? HIGH : LOW);
}

void stepper_driver_t::backward() {
  digitalWrite(dir_pin, inverted_dir ? LOW : HIGH);
}

void stepper_driver_t::steps(uint16_t steps, uint16_t us_delay = 800) {
    
    for (uint16_t i = 0; i < steps; i++) {
      digitalWrite(step_pin, HIGH);
      delayMicroseconds(us_delay);
      digitalWrite(step_pin, LOW);
      delayMicroseconds(us_delay);
    }

    if (digitalRead(dir_pin) == inverted_dir ? HIGH : LOW) {
      current_step += steps;
    }
    else {
      current_step -= steps;
    }
  }
