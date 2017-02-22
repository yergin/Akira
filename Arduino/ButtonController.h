#pragma once

#include <Bounce2.h>
#include <cinttypes>

class Momentary : public Bounce
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
    USER_EVENT,
    EVENT_COUNT
  };

  class Listener
  {
  public:
    virtual void buttonEvent(Momentary* button, Event event) = 0;
  };
  
  Momentary() {}

  void update();
  void reset() { _gestureEvents = 0; }
  
  bool isDown() { return read() != _inverted; }
  bool wasPressed() { return _inverted ? fell() : rose(); }
  bool wasReleased() { return _inverted ? rose() : fell(); }
  bool triggered() const { return _currentEvents != 0; }
  bool triggered(Event event) const { return _currentEvents & (1 << static_cast<int>(event)); }
  void triggerUserEvent() { triggerEvent(USER_EVENT); }
  bool gestureStarted() const { return _gestureEvents != 0; }
  bool gestureIncludes(Event event) const { return _gestureEvents & (1 << static_cast<int>(event)); }
  
  void setHoldTime(unsigned int ms) { _holdTime = ms; }
  unsigned int holdTime() { return _holdTime; }
  void setDoubleTapTime(unsigned int ms) { _doubleTapTime = ms; }
  unsigned int doubleTapTime() { return _doubleTapTime; }
  void setInverted(bool inverted) { _inverted = inverted; }
  bool isInverted() const { return _inverted; }
  void setListener(Listener* listener) { _listener = listener; }

private:
  void triggerEvent(Event event);
  void clearEvents() { _currentEvents = 0; }

  Listener* _listener = 0;
  bool _inverted = true;
  unsigned int _holdTime = 400;
  unsigned int _doubleTapTime = 150;
  bool _reset = false;
  uint8_t _currentEvents = 0;
  uint8_t _gestureEvents = 0;
};

