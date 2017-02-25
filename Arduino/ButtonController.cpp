#include "ButtonController.h"
#include "Arduino.h"

#define SERIAL_DEBUG

void Momentary::update() {
  unsigned long elasped = millis() - previous_millis;
    
  if (_reset) {
    reset();
  }
  
  Bounce::update();

  clearEvents();

  if (wasPressed()) {
    triggerEvent(PRESS);
      
    if (gestureIncludes(RELEASE) && elasped < _doubleTapTime) {
      triggerEvent(DOUBLE_TAP);
    }
  }
  else if (wasReleased()) {
    if (!gestureStarted()) {
      return;
    }
    triggerEvent(RELEASE);
      
    if (gestureIncludes(HOLD)) {
      triggerEvent(LONG_RELEASE);
      _reset = true;
    }
    else if (gestureIncludes(DOUBLE_TAP)) {
      _reset = true;
    }
    else {
      triggerEvent(SHORT_RELEASE);
    }
  }
  else if (isDown()) {
    if (gestureStarted() && !gestureIncludes(HOLD) && !gestureIncludes(RELEASE) && elasped >= _holdTime) {
      triggerEvent(HOLD);
    }
  }
  else {
    if (gestureIncludes(PRESS) && elasped >= _doubleTapTime) {
      triggerEvent(SINGLE_TAP);
      _reset = true;
    }
  }
}

void Momentary::reset()
{
  _currentEvents = 0;
  _gestureEvents = 0;
  _reset = false;
}

void Momentary::wakeup() {
  previous_millis = millis();
}

void Momentary::triggerEvent(Event event) {
  _currentEvents |= 1 << static_cast<int>(event);
  _gestureEvents |= 1 << static_cast<int>(event);
  
#ifdef SERIAL_DEBUG
  Serial.print("Momentary: Triggering event: ");
  switch (event) {
    case PRESS: Serial.print("PRESS\n"); break;
    case RELEASE: Serial.print("RELEASE\n"); break;
    case SHORT_RELEASE: Serial.print("SHORT_RELEASE\n"); break;
    case SINGLE_TAP: Serial.print("SINGLE_TAP\n"); break;
    case DOUBLE_TAP: Serial.print("DOUBLE_TAP\n"); break;
    case HOLD: Serial.print("HOLD\n"); break;
    case LONG_RELEASE: Serial.print("LONG_RELEASE\n"); break;
    case USER_EVENT: Serial.print("USER_EVENT\n"); break;
    case EVENT_COUNT: break;
  }
#endif

  if (_listener) {
    _listener->buttonEvent(this, event);
  }
}
