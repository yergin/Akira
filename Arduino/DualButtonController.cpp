#include "DualButtonController.h"
#include <Arduino.h>

DualButtonController::DualButtonController(int pinA, int pinB) {
  _button1.attach(pinA, INPUT_PULLUP);
  _button2.attach(pinB, INPUT_PULLUP);
  _buttonA = &_button1;
  _buttonB = &_button2;

  updateState();
}

void DualButtonController::updateState() {
  _state = 0;
  _state |= _buttonA->isDown() ? A_DOWN_MASK : A_UP_MASK;
  _state |= _buttonA->isHeld() ? A_HOLD_MASK : 0;
  _state |= _buttonB->isDown() ? B_DOWN_MASK : B_UP_MASK;
  _state |= _buttonB->isHeld() ? B_HOLD_MASK : 0;
}

void DualButtonController::updateEventVector() {

}

void DualButtonController::update() {
  _buttonA->update();
  _buttonB->update();
  updateState();
  updateEventVector();
}

void DualButtonController::swapButtons() {
  _swapButtonsOnRelease = false;
  _buttonsSwapped = !_buttonsSwapped;
  
  _buttonA = _buttonsSwapped ? &_button2 : &_button1;
  _buttonB = _buttonsSwapped ? &_button1 : &_button2;
}

bool DualButtonController::eventDidOccur(Event event) const {
  return _eventVector & (0x1 << static_cast<int>(event));
}

void DualButtonController::performRequest(Request request) {
  switch (request) {
    case SKIP_RELEASE_EVENT:
      if (_state & (A_DOWN_MASK | B_DOWN_MASK)) {
        _skipReleaseEvent = true;
      }
    case SKIP_HOLD_EVENT:
      if (_state & (A_HOLD_MASK | A_HOLD_MASK)) {
        _skipHoldEvent = true;
      }
    case SWAP_BUTTONS_ON_RELEASE:
      if (_state & (A_DOWN_MASK | B_DOWN_MASK)) {
        _swapButtonsOnRelease = true;
      }
      else {
        swapButtons();
      }
      
  }
}

