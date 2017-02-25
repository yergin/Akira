#include "DualButtonController.h"
#include "Config.h"
#include <Arduino.h>

DualButtons::Controller Buttons(BUTTON_A_PIN, BUTTON_B_PIN);

namespace DualButtons {

Controller::Controller(int pinA, int pinB) {
  _button1.attach(pinA, INPUT_PULLUP);
  _button2.attach(pinB, INPUT_PULLUP);
  _buttonA = &_button1;
  _buttonB = &_button2;
  _buttonA->update();
  _buttonB->update();
}

void Controller::update() {
  unsigned int ms = millis();
  
  clearEventsForButtonsABTogether();
  _buttonA->update();
  _buttonB->update();

  bool anyButtonJustPressed = _buttonA->triggered(Momentary::PRESS) || _buttonB->triggered(Momentary::PRESS);
  bool bothButtonsJustPressed = _buttonA->triggered(Momentary::PRESS) && _buttonB->triggered(Momentary::PRESS);
  bool anyButtonDown = _buttonA->isDown() || _buttonB->isDown();
  bool bothButtonsDown = _buttonA->isDown() && _buttonB->isDown();

  if (bothButtonsJustPressed) {
    triggerEventForButtonsABTogether(PRESS);
    _firstButtonPressed = ms;
    _secondButtonPressed = ms;
  }
  else if (anyButtonJustPressed) {
    if (bothButtonsDown) {
      _secondButtonPressed = ms;
      if (ms <= _firstButtonPressed + _simultaneousPressThreshold) {
        triggerEventForButtonsABTogether(PRESS);
      }
    }
    else {
      _firstButtonPressed = ms;
    }
  }
  else if (bothButtonsDown) {
    if (!gestureIncludes(BUTTONS_A_B_TOGETHER, HOLD) && gestureIncludes(BUTTONS_A_B_TOGETHER, PRESS) &&
                                                        ms >= _secondButtonPressed + _simultaneousHoldTime) {
      triggerEventForButtonsABTogether(HOLD);
    }
  }
  else if (anyButtonDown) {
    Momentary* downBtn = _buttonA->isDown() ? _buttonA : _buttonB;
    Momentary* upBtn = _buttonA->isDown() ? _buttonB : _buttonA;
    if (!downBtn->gestureIncludes(Momentary::USER_EVENT) && !upBtn->gestureIncludes(Momentary::USER_EVENT) && 
        !gestureIncludes(BUTTONS_A_B_TOGETHER, PRESS) && ms > _firstButtonPressed + _simultaneousPressThreshold) {
      downBtn->triggerUserEvent();
    }
  }
  else if (_buttonA->triggered(Momentary::RELEASE) || _buttonB->triggered(Momentary::RELEASE)) {
    if (gestureIncludes(BUTTONS_A_B_TOGETHER, PRESS)) {
      triggerEventForButtonsABTogether(RELEASE);
    }
  }
  else {
    resetButtonsABTogether();
    if (_swapButtonsOnRelease && !_buttonA->gestureStarted() && !_buttonB->gestureStarted()) {
      swapButtons();
    }
  }
}

void Controller::reset() {
  _buttonA->reset();
  _buttonB->reset();
  resetButtonsABTogether();
}

void Controller::wakeup() {
  _buttonA->wakeup();
  _buttonB->wakeup();
  if (_buttonA->isDown() || _buttonB->isDown()) {
    _firstButtonPressed = millis();
  }
  _secondButtonPressed = 0;
}

void Controller::resetButtonsABTogether() {
  _currentEventsForButtonsABTogether = 0;
  _gestureEventsForButtonsABTogether = 0;
}

void Controller::swapButtons() {
  _swapButtonsOnRelease = false;
  _buttonsSwapped = !_buttonsSwapped;
  
  _buttonA = _buttonsSwapped ? &_button2 : &_button1;
  _buttonB = _buttonsSwapped ? &_button1 : &_button2;
}

bool Controller::triggered(Button button) const
{
  switch (button) {
    case BUTTON_A: return _buttonA->triggered();
    case BUTTON_B: return _buttonB->triggered();
    default:
      break;
  }

  return _currentEventsForButtonsABTogether != 0;
}

bool Controller::triggered(Button button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->triggered((Momentary::Event)event);
    case BUTTON_B: return _buttonB->triggered((Momentary::Event)event);
    default:
      break;
  }

  return _currentEventsForButtonsABTogether & (1 << static_cast<int>(event));;
}

bool Controller::gestureIncludes(Button button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->gestureIncludes((Momentary::Event)event);
    case BUTTON_B: return _buttonB->gestureIncludes((Momentary::Event)event);
    default:
      break;
  }

  return _gestureEventsForButtonsABTogether & (1 << static_cast<int>(event));
}

bool Controller::gestureStarted() const {
  return gestureStarted(BUTTON_A) || gestureStarted(BUTTON_B) || gestureStarted(BUTTONS_A_B_TOGETHER);
}

bool Controller::gestureStarted(Button button) const {
  switch (button) {
    case BUTTON_A: return _buttonA->gestureStarted();
    case BUTTON_B: return _buttonB->gestureStarted();
    default:
      break;
  }

  return _gestureEventsForButtonsABTogether != 0;  
}

void Controller::triggerEventForButtonsABTogether(Event event) {
  _currentEventsForButtonsABTogether |= 1 << static_cast<int>(event);
  _gestureEventsForButtonsABTogether |= 1 << static_cast<int>(event);
#ifdef SERIAL_DEBUG
  Serial.print("Dual Buttons: Triggering event: ");
  switch (event) {
    case PRESS: Serial.println("PRESS"); break;
    case RELEASE: Serial.println("RELEASE"); break;
    case HOLD: Serial.println("HOLD"); break;
    default: break;
  }
#endif
}

bool Controller::isButtonDown(Button button) const {
  switch (button) {
    case BUTTON_A: return _buttonA->isDown();
    case BUTTON_B: return _buttonB->isDown();
    default: break;
  }
  return _buttonA->isDown() && _buttonB->isDown();
}

void Controller::performRequest(Request request) {
  switch (request) {
    case RESET_BUTTONS:
      reset();
      break;
      
    case SWAP_BUTTONS_ON_RELEASE:
      if (_buttonA->gestureStarted() || _buttonB->gestureStarted()) {
        _swapButtonsOnRelease = true;
      }
      else {
        swapButtons();
      }
    default:
      break;
  }
}

}

