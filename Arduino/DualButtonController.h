#pragma once

#include "YablButton.h"

namespace DualButtons {

static constexpr Event EXCLUSIVE_PRESS = USER_EVENT;

enum ButtonId {
  BUTTON_A = 0,
  BUTTON_B,
  BUTTONS_A_B_TOGETHER
};

enum Request {
  NO_BUTTON_REQUEST = 0,
  RESET_BUTTONS,
  SWAP_BUTTONS_ON_RELEASE
};

using namespace Yabl;

class Controller
{
public:
  Controller(int pinA, int pinB);

  bool activity() const { return activity(BUTTON_A) || activity(BUTTON_B) || activity(BUTTONS_A_B_TOGETHER); }
  bool activity(ButtonId button) const;
  bool triggered(ButtonId button, Event event) const;
  bool gestureIncludes(ButtonId button, Event event) const;
  bool gestureStarted() const;
  bool gestureStarted(ButtonId button) const;
  void performRequest(Request request);
  bool areButtonsSwapped() const { return _buttonsSwapped; }
  bool isButtonDown(ButtonId button) const;
  void update();
  void reset();
  void sleep() { reset(); }
  void wakeup(); 

protected:
    static void onButtonEvent(Button& button, Event event);

private:
  void swapButtons();
  void triggerEventForButtonsABTogether(Event event);
  void clearEventsForButtonsABTogether() { _currentEventsForButtonsABTogether = 0; }
  void resetButtonsABTogether();
  
  Button _button1;
  Button _button2;
  Button* _buttonA = 0;
  Button* _buttonB = 0;
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
