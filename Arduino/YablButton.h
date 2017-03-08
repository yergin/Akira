#pragma once

#include <Bounce2.h>
#include <cinttypes>

namespace Yabl {

typedef uint16_t Event;

static constexpr Event PRESS = 0x01;
static constexpr Event RELEASE = 0x02;
static constexpr Event SHORT_RELEASE = 0x04;
static constexpr Event SINGLE_TAP = 0x08;
static constexpr Event DOUBLE_TAP = 0x10;
static constexpr Event HOLD = 0x20;
static constexpr Event LONG_RELEASE = 0x40;
static constexpr Event USER_EVENT = 0x80;
static constexpr Event ALL_EVENTS = 0xFFFF;

class Button : public Bounce
{
public:
  typedef void (*CallbackSimple)();
  typedef void (*CallbackWithButtonAndEvent)(Button&, Event);
  
  Button() {}

  void update();
  void reset();
  void sleep() { reset(); }
  void wakeup();
  
  bool down() { return read() != _inverted; }
  bool pressed() { return _inverted ? fell() : rose(); }
  bool released() { return _inverted ? rose() : fell(); }
  bool activity() const { return _currentEvents != 0; }
  bool triggered(Event event) const { return _currentEvents & event; }
  void triggerUserEvent() { triggerEvent(USER_EVENT); }
  bool gestureStarted() const { return _gestureEvents != 0; }
  bool gestureIncludes(Event event) const { return _gestureEvents & event; }
  void suppressOnce(Event event) { _suppressEvents |= event; }
  
  void setHoldTime(unsigned int ms) { _holdTime = ms; }
  unsigned int holdTime() const { return _holdTime; }
  void setDoubleTapTime(unsigned int ms) { _doubleTapTime = ms; }
  unsigned int doubleTapTime() const { return _doubleTapTime; }
  void setInverted(bool inverted) { _inverted = inverted; }
  bool inverted() const { return _inverted; }
  void setCallback(CallbackSimple callback, Event forEvents);
  void setCallback(CallbackWithButtonAndEvent callback, Event forEvents = ALL_EVENTS);

  bool operator==(const Button& other) const { return this == &other; }
  
private:
  struct Callback {
    Callback() : type(NONE) {}
    enum {
      NONE = 0,
      SIMPLE,
      WITH_BUTTON_AND_EVENT
    } type;
    union {
      CallbackSimple simple;
      CallbackWithButtonAndEvent withButtonAndEvent;
    } callback;
  };
  
  void triggerEvent(Event event);
  void clearEvents() { _currentEvents = 0; }

  static constexpr int EVENT_COUNT = 16;

  bool _inverted = true;
  unsigned int _holdTime = 400;
  unsigned int _doubleTapTime = 150;
  bool _reset = false;
  Event _currentEvents = 0;
  Event _gestureEvents = 0;
  Event _suppressEvents = 0;
  Callback _callbacks[EVENT_COUNT] = {};
};

} // namespace Yabl

using namespace Yabl;
