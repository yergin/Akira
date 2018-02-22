#include "ErrorStatus.h"

void ErrorStatus::setError(int code) {
  _errorVector |= 1ul << code;
  
  if (isCodeBeingDisplayed()) {
    if (code < currentCode()) {
      _repeatCodeDisplay = true;
    }
  }
  else {
    displayNextCode();
  }
}

void ErrorStatus::displayNextCode() {
  if (_currentCode == -1) {
    _repeatCodeDisplay = false;
  }
  
  _currentCode = nextCode();
  _codeDisplayStartTime = _currentCode == -1 ? 0 : millis();
}

int ErrorStatus::nextCode() const {
  for (int i = _currentCode + 1; i < MAX_CODES; ++i) {
    if (isErrorSet(i)) {
      return i;
    }
  }
  return -1;
}

void ErrorStatus::update() {
  unsigned long ms = millis();

  if (!isCodeBeingDisplayed()) {
    if (_codeDisplayStartTime == 0 || ms < _codeDisplayStartTime) {
      return;
    }

    displayNextCode();
  }
  
  unsigned long t = ms - _codeDisplayStartTime;
  unsigned long tBurstStop = currentCodeBurstCount() * (BURST_DURATION + PAUSE);
  unsigned long tPulseStop = tBurstStop + currentCodePulseCount() * PULSE_DURATION;
  unsigned long tTerminatorStart = tPulseStop + (tPulseStop ? LONG_DUTY : 0);
  unsigned long tStop = tTerminatorStart + LONG_DUTY * 3;

  if (t >= tStop) {
    displayNextCode();
    if (currentCode() == -1) {
      if (_repeatCodeDisplay) {
        _repeatCodeDisplay = false;
        _codeDisplayStartTime = ms;
        t = 0;
      }
      else {
        _codeDisplayStartTime = ms + RESTART_GAP;
        return;
      }
    }
  }
  
  if (t < tBurstStop) {
    bool inBurst = t % (BURST_DURATION + PAUSE) < BURST_DURATION;
    digitalWrite(STATUS_LED_PIN, inBurst && t % (FAST_SHORT_DUTY * 2) < FAST_SHORT_DUTY);
  }
  else if (t < tPulseStop) {
    digitalWrite(STATUS_LED_PIN, (t - tBurstStop) % (SLOW_SHORT_DUTY * 2) < SLOW_SHORT_DUTY);
  }
  else if (t < tStop) {
    digitalWrite(STATUS_LED_PIN, t > tTerminatorStart && (t - tTerminatorStart) < LONG_DUTY);
  }
  else {
    digitalWrite(STATUS_LED_PIN, LOW);
  }
}

void ErrorStatus::resetDisplay() {
  digitalWrite(STATUS_LED_PIN, LOW);
  _currentCode = -1;
}

ErrorStatus Errors;
