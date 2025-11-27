#include "arcade_buttons.h"


void arcade_button_t::init() {
  pinMode(pin, INPUT_PULLUP);
  last_state = digitalRead(pin);
  last_change_ms = millis();
}

bool arcade_button_t::is_pressed()
{
  int raw = digitalRead(pin);           // Read the current raw state (HIGH or LOW)
  unsigned long now = millis();

  if (raw != last_state)
  {
    // reset debounce timer
    last_change_ms = now;
    last_state = raw;
  }

  // If stable long enough, consider it debounced
  if ((now - last_change_ms) > debounce_ms)
  {
    // Button is active LOW, so return true only if LOW
    return (last_state == LOW);
  }

  return false;  // not stable yet
}

bool arcade_button_t::is_held() {
  return (digitalRead(pin) == LOW);
}