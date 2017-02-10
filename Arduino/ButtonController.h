#pragma once

#include <Bounce2.h>

class ButtonController : public Bounce
{
public:
  enum Event {
    PRESS = 0,
    SHORT_RELEASE,
    LONG_RELEASE,
    HOLD
  };

  typedef int State;
  static constexpr State UP_MASK = 0x01;
  static constexpr State DOWN_MASK = 0x02;
  static constexpr State HELD_MASK = 0x04;
  
  static constexpr State UP = UP_MASK;
  static constexpr State DOWN = DOWN_MASK;
  static constexpr State DOWN_AND_HELD = DOWN_MASK | HELD_MASK;
  
  class Listener
  {
  public:
    virtual void buttonEvent(Event event, State state) = 0;
  };
  
  ButtonController();

  void update();
  
  bool isDown() { return read() ^ _inverted; }
  bool isHeld() { return _hold; }
  
  bool wasPressed() { return _inverted ? fell() : rose(); }
  bool wasShortRelease() { return wasReleased() && !_wasLongRelease; }
  bool wasLongRelease() { return wasReleased() && _wasLongRelease; }
  bool wasReleased() { return !_skipReleaseEvent && (_inverted ? rose() : fell()); }
  bool wasHeld() { return !_skipHoldEvent && (_hold && _heldSet); }
  
  void setListener(Listener* listener);
  void skipReleaseEvent();
  void skipHoldEvent();

  void setHoldTime(unsigned int ms);
  void setInverted(bool inverted);

private:
  Listener* _listener = 0;
  bool _inverted = true;
  bool _hold = false;
  bool _heldSet = false;
  unsigned int _holdTime = 500;
  bool _skipReleaseEvent = false;
  bool _skipHoldEvent = false;
  bool _wasLongRelease = false;
};

