#pragma once

#include <Bounce2.h>
#include <cinttypes>

class ButtonController : public Bounce
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
    USER_EVENT
  };

  class Listener
  {
  public:
    virtual void buttonEvent(Event event, ButtonController* controller) = 0;
  };
  
  ButtonController() {}

  void update();
  
  bool isDown() { return read() ^ _inverted; }
  bool wasPressed() { return _inverted ? fell() : rose(); }
  bool wasReleased() { return _inverted ? rose() : fell(); }
  bool justOccurred(Event event) const { return _currentEvents & (1 << static_cast<int>(event)); }
  bool hasActionStarted() const { return _actionEvents != 0; }
  bool didOccurInAction(Event event) const { return _actionEvents & (1 << static_cast<int>(event)); }
  
  void setHoldTime(unsigned int ms) { _holdTime = ms; }
  void setDoubleTapTime(unsigned int ms) { _doubleTapTime = ms; }
  void setInverted(bool inverted) { _inverted = inverted; }
  void setListener(Listener* listener) { _listener = listener; }
  void resetAction() { _actionEvents = 0; }
  void setUserEvent() { setEvent(USER_EVENT); }

private:
  void setEvent(Event event);
  void clearEvents() { _currentEvents = 0; }

  Listener* _listener = 0;
  bool _inverted = true;
  unsigned int _holdTime = 300;
  unsigned int _doubleTapTime = 150;
  uint8_t _currentEvents = 0;
  uint8_t _actionEvents = 0;
  bool _resetAction = false;
};

