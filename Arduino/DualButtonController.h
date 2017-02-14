#pragma once

#include "ButtonController.h"

class DualButtonController
{
public:
  enum Event {
    PRESS = 0,
    RELEASE,
    SHORT_RELEASE,
    SINGLE_TAP,
    DOUBLE_TAP,
    HOLD,
    LONG_RELEASE,
    EXCLUSIVE_PRESS
  };

  enum Button {
    BUTTON_A = 0,
    BUTTON_B,
    BOTH_BUTTONS
  };

  enum Request {
    RESET_BUTTON_ACTIONS,
    SWAP_BUTTONS_ON_RELEASE
  };

  DualButtonController(int pinA, int pinB);
  
  bool justOccurred(Button button, Event event) const;
  bool didOccurInAction(Button button, Event event) const;
  void performRequest(Request request);
  bool areButtonsSwapped() const { return _buttonsSwapped; }
  void update();

private:
  void swapButtons();
  void setEventForBothButtons(Event event);
  void clearEventsForBothButtons() { _currentEventsForBothButtons = 0; }
  void resetActionsForBothButtons() { _actionEventsForBothButtons = 0; }
  
  ButtonController _button1;
  ButtonController _button2;
  ButtonController* _buttonA = 0;
  ButtonController* _buttonB = 0;
  int _simultaneousThreshold = 100;
  bool _buttonsSwapped = false;
  bool _swapButtonsOnRelease = false;
  unsigned long _currentEventsForBothButtons = 0;
  unsigned long _actionEventsForBothButtons = 0;
  unsigned long _firstButtonPressed = 0;
};

