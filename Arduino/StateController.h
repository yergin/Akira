#pragma once

#include "StateTransitionTable.h"
#include <cinttypes>

class OperatingMode {
public:
  virtual void initialize() {}
  virtual void enter(Command command) { execute(command); }
  virtual void execute(Command command);
  virtual void leave() {}
  virtual const char* name() = 0;
};

class OffMode : public OperatingMode {
public:
  void enter(Command command);
  void leave();
  const char* name() { return "OFF"; }
  
private:
  void sleep();
  void wakeUp();
  bool areLedsOn() const;
};

class PerformMode : public OperatingMode {
public:
  const char* name() { return "PERFORM"; }
};

class BrightnessMode : public OperatingMode {
public:
  void initialize();
  void execute(Command command);
  void leave();
  const char* name() { return "BRIGHTNESS"; }
  
private:
  static constexpr uint8_t _levels[] = { 7, 12, 20, 31, 45, 63, 90, 127, 181, 255 };
  static int numLevels() { return sizeof(_levels) / sizeof(_levels[0]); }
  
  uint8_t level() const { return _levels[_currentLevel]; }
  
  int _currentLevel = 2; // serialise
};

class ProgramEnterMode : public OperatingMode {
public:
  const char* name() { return "PROGRAM ENTER"; }
};

class ProgramMode : public OperatingMode {
public:
  const char* name() { return "PROGRAM"; }
};

class StateController {
public:
  static int transitionTableEntryCount() { return sizeof(stateTransitionTable) / sizeof(StateTransition); }

  void initializeModes();
  void setOperatingMode(Mode mode) { setOperatingModeWithCommand(mode, DO_NOTHING); }
  void setOperatingModeWithCommand(Mode mode, Command command);
  Mode currentMode() const { return static_cast<Mode>(_currentMode); }
  
private:
  int _currentMode = -1;
  static OperatingMode* _modes[];
};

extern StateController Akira;
