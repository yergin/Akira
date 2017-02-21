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
  BUTTONS_A_B_TOGETHER
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

  bool triggered() const { return triggered(BUTTON_A) || triggered(BUTTON_B) || triggered(BUTTONS_A_B_TOGETHER); }
  bool triggered(Button button) const;
  bool triggered(Button button, Event event) const;
  bool gestureIncludes(Button button, Event event) const;
  void performRequest(Request request);
  bool areButtonsSwapped() const { return _buttonsSwapped; }
  bool isButtonDown(Button button) const;
  void update();
  void reset();

private:
  void swapButtons();
  void triggerEventForButtonsABTogether(Event event);
  void clearEventsForButtonsABTogether() { _currentEventsForButtonsABTogether = 0; }
  void resetButtonsABTogether() { _gestureEventsForButtonsABTogether = 0; }
  
  Momentary _button1;
  Momentary _button2;
  Momentary* _buttonA = 0;
  Momentary* _buttonB = 0;
  int _simultaneousPressThreshold = 100;
  int _simultaneousHoldTime = 300;
  bool _buttonsSwapped = false;
  bool _swapButtonsOnRelease = false;
  unsigned long _currentEventsForButtonsABTogether = 0;
  unsigned long _gestureEventsForButtonsABTogether = 0;
  unsigned long _firstButtonPressed = 0;
  unsigned long _secondButtonPressed = 0;
};

}

extern DualButtons::Controller Buttons;
