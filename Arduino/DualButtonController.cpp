#include "DualButtonController.h"
#include <Arduino.h>

namespace DualButtons {

Controller::Controller(int pinA, int pinB) {
  _button1.attach(pinA, INPUT_PULLUP);
  _button2.attach(pinB, INPUT_PULLUP);
  _buttonA = &_button1;
  _buttonB = &_button2;
}

void Controller::update() {
  bool gestureStarted = _buttonA->gestureStarted() || _buttonB->gestureStarted();

  _buttonA->update();
  _buttonB->update();

  bool anyButtonJustPressed = _buttonA->triggered(Momentary::PRESS) || _buttonA->triggered(Momentary::PRESS);
  bool bothButtonsJustPressed = _buttonA->triggered(Momentary::PRESS) && _buttonA->triggered(Momentary::PRESS);

  if (bothButtonsJustPressed) {
    triggerEventForBothButtons(PRESS);
  }
  else if (anyButtonJustPressed) {
    
  }
  
  /*
  if (!hasActionStarted && _buttonA->justOccured(PRESS) || _buttonB->justOccured(PRESS)) {
    _firstButtonPressed = millis();
  }
  else if (_buttonA->justOccured(RELEASE) || _buttonB->justOccured(RELEASE)) {
    _firstButtonPressed = 0;
  }

  if (_firstButtonPressed > 0) {
    if (millis() > _firstButtonPressed + _simultaneousThreshold) {
      if (!_buttonA->didOccurInAction(PRESS)) {
        _buttonA->setEvent(USER_EVENT);
      }
      else if (!_buttonB->didOccurInAction(PRESS)) {
        _buttonB->setEvent(USER_EVENT);
      }
    }
    else if (_buttonA->isDown() && _buttonB->isDown())
      setEventForBothButtons(PRESS);
    }
  }
  */
}

void Controller::swapButtons() {
  _swapButtonsOnRelease = false;
  _buttonsSwapped = !_buttonsSwapped;
  
  _buttonA = _buttonsSwapped ? &_button2 : &_button1;
  _buttonB = _buttonsSwapped ? &_button1 : &_button2;
}

bool Controller::triggered(Button button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->triggered((Momentary::Event)event);
    case BUTTON_B: return _buttonB->triggered((Momentary::Event)event);
    default:
      break;
  }

  return _currentEventsForBothButtons & (1 << static_cast<int>(event));;
}

bool Controller::gestureIncludes(Button button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->gestureIncludes((Momentary::Event)event);
    case BUTTON_B: return _buttonB->gestureIncludes((Momentary::Event)event);
    default:
      break;
  }

  return _gestureEventsForBothButtons & (1 << static_cast<int>(event));;
}

void Controller::triggerEventForBothButtons(Event event) {
  _currentEventsForBothButtons |= 1 << static_cast<int>(event);
  _gestureEventsForBothButtons |= 1 << static_cast<int>(event);  
}

void Controller::performRequest(Request request) {
  switch (request) {
    case RESET_BUTTONS:
      resetBothButtons();
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

