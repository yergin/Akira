#pragma once

#include "StateTransitionTable.h"
#include "Animations.h"
#include <cinttypes>

class OperatingMode {
public:
  virtual void initialize() {}
  virtual void enter(Command command) { execute(command); }
  virtual void execute(Command command);
  virtual void leave() {}
  virtual const char* name() = 0;

protected:
  void shortBlink();
  void longBlink();
};

class SleepMode : public OperatingMode {
public:
  void enter(Command command);
  void leave();
  const char* name() { return "OFF"; }
  
private:
  void sleep();
  void wakeUp();
};

class PerformMode : public OperatingMode {
public:
  const char* name() { return "PERFORM"; }
};

class BrightnessMode : public OperatingMode {
public:
  void initialize();
  void enter(Command command);
  void execute(Command command);
  void leave();
  const char* name() { return "BRIGHTNESS"; }
  void reset();
  
private:
  static constexpr uint8_t _levels[] = { 7, 12, 20, 31, 45, 63, 90, 127, 181, 255 };
  static int numLevels() { return sizeof(_levels) / sizeof(_levels[0]); }
  static constexpr int DEFAULT_SETTING = 5;
  
  uint8_t level() const { return _levels[_currentSetting]; }
  void save();
  void load();
  void apply();
  
  int _currentSetting = DEFAULT_SETTING;
};

class ProgramEnterMode : public OperatingMode {
public:
  void enter(Command command);
  void execute(Command command);
  const char* name() { return "PROGRAM ENTER"; }
};

class ProgramMode : public OperatingMode {
public:
  void enter(Command command);
  void execute(Command command);
  void leave();
  const char* name() { return "PROGRAM"; }
};

class StateController {
public:
  static int transitionTableEntryCount() { return sizeof(stateTransitionTable) / sizeof(StateTransition); }

  void initialize();
  void update();
  bool areLedsOn() const;
  void turnLedsOn();
  void turnLedsOff();
  void setOperatingMode(Mode mode) { setOperatingModeWithCommand(mode, DO_NOTHING); }
  void setOperatingModeWithCommand(Mode mode, Command command);
  Mode currentMode() const { return static_cast<Mode>(_currentMode); }

  void showFirstCue();
  void showNextCue();
  void showPreviousCue();
  void enterProgramMode();
  void deleteCurrentCue();
  void insertCue();
  void eraseAllCues();
  void changeColor1();
  void changeColor2();
  void changeAnimation();
  void exitProgramMode();
  
private:
  void factoryReset();
  void storeDemoCues();
  void setNextAnimation(AkiraAnimation* animation);
  AkiraAnimation* createAnimation(int index);
  Transition* transition() const;
  void loadCuesFromEeprom();
  void storeCuesInEeprom();

  AkiraAnimation* _targetAnimation = 0;
  AkiraAnimation* _sourceAnimation = 0;
  int _currentCue = -1;
  int _currentMode = -1;
  static OperatingMode* _modes[];
  AkiraAnimation::CueDescription _cues[MAX_CUES];
  int _cueCount;
};

extern StateController Akira;
