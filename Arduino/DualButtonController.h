#pragma once

#include "ButtonController.h"

namespace DualButtons {

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
  NO_BUTTON_REQUEST = 0,
  RESET_BUTTONS,
  SWAP_BUTTONS_ON_RELEASE
};

class Controller
{
public:
  Controller(int pinA, int pinB);
  
  bool triggered(Button button, Event event) const;
  bool gestureIncludes(Button button, Event event) const;
  void performRequest(Request request);
  bool areButtonsSwapped() const { return _buttonsSwapped; }
  void update();

private:
  void swapButtons();
  void triggerEventForBothButtons(Event event);
  void clearEventsForBothButtons() { _currentEventsForBothButtons = 0; }
  void resetBothButtons() { _gestureEventsForBothButtons = 0; }
  
  Momentary _button1;
  Momentary _button2;
  Momentary* _buttonA = 0;
  Momentary* _buttonB = 0;
  int _simultaneousThreshold = 100;
  bool _buttonsSwapped = false;
  bool _swapButtonsOnRelease = false;
  unsigned long _currentEventsForBothButtons = 0;
  unsigned long _gestureEventsForBothButtons = 0;
  unsigned long _firstButtonPressed = 0;
};

}
