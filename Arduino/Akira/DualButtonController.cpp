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
  _buttonA->callback(onButtonEvent);
  _buttonB->callback(onButtonEvent);
}

void Controller::update() {
  unsigned int ms = millis();
  
  clearEventsForButtonsABTogether();
  _buttonA->update();
  _buttonB->update();

  bool anyButtonJustPressed = _buttonA->triggered(PRESS) || _buttonB->triggered(PRESS);
  bool bothButtonsJustPressed = _buttonA->triggered(PRESS) && _buttonB->triggered(PRESS);
  bool anyButtonDown = _buttonA->down() || _buttonB->down();
  bool bothButtonsDown = _buttonA->down() && _buttonB->down();

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
      _buttonA->suppressOnce(HOLD);
      _buttonB->suppressOnce(HOLD);
      _buttonA->suppressOnce(SINGLE_TAP);
      _buttonB->suppressOnce(SINGLE_TAP);
      triggerEventForButtonsABTogether(HOLD);
    }
  }
  else if (anyButtonDown) {
    SingleButton* downBtn = _buttonA->down() ? _buttonA : _buttonB;
    SingleButton* upBtn = _buttonA->down() ? _buttonB : _buttonA;
    if (!downBtn->gestureIncludes(EXCLUSIVE_PRESS) && !upBtn->gestureIncludes(EXCLUSIVE_PRESS) && 
        !gestureIncludes(BUTTONS_A_B_TOGETHER, PRESS) && ms > _firstButtonPressed + _simultaneousPressThreshold) {
      downBtn->triggerExclusivePress();
    }
  }
  else if (_buttonA->triggered(RELEASE) || _buttonB->triggered(RELEASE)) {
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
  if (_buttonA->down() || _buttonB->down()) {
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

bool Controller::activity(ButtonId button) const
{
  switch (button) {
    case BUTTON_A: return _buttonA->activity();
    case BUTTON_B: return _buttonB->activity();
    default:
      break;
  }

  return _currentEventsForButtonsABTogether != 0;
}

bool Controller::triggered(ButtonId button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->triggered(event);
    case BUTTON_B: return _buttonB->triggered(event);
    default:
      break;
  }

  return _currentEventsForButtonsABTogether & event;
}

bool Controller::gestureIncludes(ButtonId button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->gestureIncludes(event);
    case BUTTON_B: return _buttonB->gestureIncludes(event);
    default:
      break;
  }

  return _gestureEventsForButtonsABTogether & event;
}

bool Controller::gestureStarted() const {
  return gestureStarted(BUTTON_A) || gestureStarted(BUTTON_B) || gestureStarted(BUTTONS_A_B_TOGETHER);
}

bool Controller::gestureStarted(ButtonId button) const {
  switch (button) {
    case BUTTON_A: return _buttonA->gestureStarted();
    case BUTTON_B: return _buttonB->gestureStarted();
    default:
      break;
  }

  return _gestureEventsForButtonsABTogether != 0;  
}

void Controller::triggerEventForButtonsABTogether(Event event) {
  _currentEventsForButtonsABTogether |= event;
  _gestureEventsForButtonsABTogether |= event;
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

bool Controller::isButtonDown(ButtonId button) const {
  switch (button) {
    case BUTTON_A: return _buttonA->down();
    case BUTTON_B: return _buttonB->down();
    default: break;
  }
  return _buttonA->down() && _buttonB->down();
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

void Controller::onButtonEvent(const EventInfo& info) {
  Serial.print(info.button == *Buttons._buttonA ?  "Button A: " : "Button B: ");
  
  switch (info.event) {
    case PRESS: Serial.print("PRESS\n"); break;
    case RELEASE: Serial.print("RELEASE\n"); break;
    case SHORT_RELEASE: Serial.print("SHORT_RELEASE\n"); break;
    case SINGLE_TAP: Serial.print("SINGLE_TAP\n"); break;
    case DOUBLE_TAP: Serial.print("DOUBLE_TAP\n"); break;
    case HOLD: Serial.print("HOLD\n"); break;
    case LONG_RELEASE: Serial.print("LONG_RELEASE\n"); break;
    case EVENT_EXTENDED_1: Serial.print("EXCLUSIVE_PRESS\n"); break;
    default: break;
  }
}

}

