#pragma once

#include <Arduino.h>
#define STATUS_LED_PIN 2

class ErrorStatus {
public:
  static constexpr int MAX_CODES = sizeof(unsigned long) * 8;

  ErrorStatus() { pinMode(STATUS_LED_PIN, OUTPUT); }
  
  void clearAllErrors() { _errorVector = 0; }
  void clearError(int code) { _errorVector &= ~(1ul << code); }
  void setError(int code);
  bool isErrorSet(int code) const { return (_errorVector & (1ul << code)) > 0ul; }
  void resetDisplay();

  void update();

private:
  bool isCodeBeingDisplayed() const { return _currentCode >= 0; }
  unsigned long codeDisplayStartTime() const { return _codeDisplayStartTime; }
  int currentCode() const { return _currentCode; }
  int currentCodeBurstCount() const { return _currentCode / BURST_PULSES; }
  int currentCodePulseCount() const { return _currentCode % BURST_PULSES; }
  int nextCode() const;
  void displayNextCode();

  static constexpr unsigned long FAST_SHORT_DUTY = 30;
  static constexpr unsigned long SLOW_SHORT_DUTY = 100;
  static constexpr unsigned long LONG_DUTY = 400;
  static constexpr unsigned long PAUSE = 150;
  static constexpr unsigned long RESTART_GAP = 10000;
  static constexpr int BURST_PULSES = 5;
  static constexpr unsigned long BURST_DURATION = FAST_SHORT_DUTY * 2 * BURST_PULSES;
  static constexpr unsigned long PULSE_DURATION = SLOW_SHORT_DUTY * 2;
  
  unsigned long _errorVector = 0;
  unsigned long _codeDisplayStartTime = 0;
  int _currentCode = -1;
  bool _repeatCodeDisplay = false;
};

extern ErrorStatus Errors;
