#include "ButtonController.h"
#include "Arduino.h"

void ButtonController::update() {
  unsigned long elasped = millis() - previous_millis;
    
  if (_resetAction) {
    resetAction();
    _resetAction = false;
  }
  
  Bounce::update();

  clearEvents();

  if (wasPressed()) {
    setEvent(PRESS);
      
    if (didOccurInAction(RELEASE) && elasped < _doubleTapTime) {
      setEvent(DOUBLE_TAP);
    }
  }
  else if (wasReleased()) {
    if (!hasActionStarted()) {
      return;
    }
    setEvent(RELEASE);
      
    if (didOccurInAction(HOLD)) {
      setEvent(LONG_RELEASE);
      _resetAction = true;
    }
    else if (didOccurInAction(DOUBLE_TAP)) {
      _resetAction = true;
    }
    else {
      setEvent(SHORT_RELEASE);
    }
  }
  else if (isDown()) {
    if (!didOccurInAction(HOLD) && hasActionStarted() && !didOccurInAction(RELEASE) && elasped >= _holdTime) {
      setEvent(HOLD);
    }
  }
  else {
    if (didOccurInAction(PRESS) && elasped >= _doubleTapTime) {
      setEvent(SINGLE_TAP);
      _resetAction = true;
    }
  }
}

void ButtonController::setEvent(Event event) {
  _currentEvents |= 1 << static_cast<int>(event);
  _actionEvents |= 1 << static_cast<int>(event);
  if (_listener) {
    _listener->buttonEvent(event, this);
  }
}
