#include "ButtonController.h"
#include "Arduino.h"

ButtonController::ButtonController() {
}

void ButtonController::setListener(Listener* listener) {
  _listener = listener;
}

void ButtonController::setHoldTime(unsigned int ms) {
  _holdTime = ms;
}

void ButtonController::setInverted(bool inverted) {
  _inverted = inverted;
}

void ButtonController::skipReleaseEvent() {
  if (isDown()) {
    _skipReleaseEvent = true;
  }
}

void ButtonController::skipHoldEvent() {
  if (isDown() && !_hold) {
    _skipHoldEvent = true;
  }
}

void ButtonController::update() {
  Bounce::update();
  
  _heldSet = false;
  _wasLongRelease = _hold;
  
  if (isDown()) {
    if (!_hold && (millis() - previous_millis > _holdTime)) {
      _hold = true;
      _heldSet = true;
      if (_listener && _holdTime && !_skipHoldEvent) {
        _listener->buttonEvent(HOLD, DOWN_AND_HELD);
      }
    }
  }
  else {
    _hold = false;
  }

  if (_listener) {
    if (wasPressed()) {
      _listener->buttonEvent(PRESS, DOWN);
    }
    else if (wasReleased()) {
      _listener->buttonEvent(_wasLongRelease ? LONG_RELEASE : SHORT_RELEASE, UP);      
    }
  }

  if (!isDown()) {
    _skipReleaseEvent = false;
    _skipHoldEvent = false;
  }
}

