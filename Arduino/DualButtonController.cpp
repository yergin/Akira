#include "DualButtonController.h"
#include <Arduino.h>

DualButtonController::DualButtonController(int pinA, int pinB) {
  _button1.attach(pinA, INPUT_PULLUP);
  _button2.attach(pinB, INPUT_PULLUP);
  _buttonA = &_button1;
  _buttonB = &_button2;
}

void DualButtonController::update() {
  bool hasActionStarted = _buttonA->hasActionStarted() || _buttonB->hasActionStarted();
  
  _buttonA->update();
  _buttonB->update();

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

void DualButtonController::swapButtons() {
  _swapButtonsOnRelease = false;
  _buttonsSwapped = !_buttonsSwapped;
  
  _buttonA = _buttonsSwapped ? &_button2 : &_button1;
  _buttonB = _buttonsSwapped ? &_button1 : &_button2;
}

bool DualButtonController::justOccurred(Button button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->justOccurred((ButtonController::Event)event);
    case BUTTON_B: return _buttonB->justOccurred((ButtonController::Event)event);
    default:
      break;
  }

  return _currentEventsForBothButtons & (1 << static_cast<int>(event));;
}

bool DualButtonController::didOccurInAction(Button button, Event event) const {
  switch (button) {
    case BUTTON_A: return _buttonA->didOccurInAction((ButtonController::Event)event);
    case BUTTON_B: return _buttonB->didOccurInAction((ButtonController::Event)event);
    default:
      break;
  }

  return _actionEventsForBothButtons & (1 << static_cast<int>(event));;
}

void DualButtonController::setEventForBothButtons(Event event) {
  _currentEventsForBothButtons |= 1 << static_cast<int>(event);
  _actionEventsForBothButtons |= 1 << static_cast<int>(event);  
}

void DualButtonController::performRequest(Request request) {
  switch (request) {
    case RESET_BUTTON_ACTIONS:
      resetActionsForBothButtons();
      break;
      
    case SWAP_BUTTONS_ON_RELEASE:
      if (_buttonA->hasActionStarted() || _buttonB->hasActionStarted()) {
        _swapButtonsOnRelease = true;
      }
      else {
        swapButtons();
      }
  }
}

