#pragma once

#include "ButtonController.h"

class DualButtonController
{
public:
  enum Event {
    A_PRESS = 0,
    A_SHORT_RELEASE,
    A_LONG_RELEASE,
    A_HOLD,
    B_PRESS,
    B_SHORT_RELEASE,
    B_LONG_RELEASE,
    B_HOLD,
    AB_PRESS,
    AB_SHORT_RELEASE,
    AB_LONG_RELEASE,
    AB_HOLD
  };

  typedef int State;
  static constexpr State A_UP_MASK = 0x01;
  static constexpr State A_DOWN_MASK = 0x02;
  static constexpr State A_HOLD_MASK = 0x04;
  static constexpr State B_UP_MASK = 0x10;
  static constexpr State B_DOWN_MASK = 0x20;
  static constexpr State B_HOLD_MASK = 0x40;

  enum Request {
    SKIP_RELEASE_EVENT,
    SKIP_HOLD_EVENT,
    SWAP_BUTTONS_ON_RELEASE
  };

  DualButtonController(int pinA, int pinB);
  
  bool eventDidOccur(Event event) const;
  State state() const { return _state; }
  void performRequest(Request request);
  bool areButtonsSwapped() { return _buttonsSwapped; }
  void update();

private:
  void updateState();
  void updateEventVector();
  void swapButtons();
  
  ButtonController _button1;
  ButtonController _button2;
  ButtonController* _buttonA;
  ButtonController* _buttonB;
  State _state = 0;
  int _eventVector = 0;
  int _simultaneousThreshold = 100;
  bool _switchedButtons = false;
  bool _skipReleaseEvent = false;
  bool _skipHoldEvent = false;
  bool _buttonsSwapped = false;
  bool _swapButtonsOnRelease = false;
};

