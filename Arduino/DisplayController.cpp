#include "DisplayController.h"
#include <EEPROM.h>

void DisplayControllerClass::showFirstCue() {
  _currentCue = 0;
  setNextAnimation(animation(_currentCue));
}

void DisplayControllerClass::showNextCue() {
  _currentCue = _currentCue < cueCount() - 1 ? _currentCue + 1 : 0;
  setNextAnimation(animation(_currentCue));
}

void DisplayControllerClass::showPreviousCue() {
  _currentCue = _currentCue > 0 ? _currentCue - 1 : cueCount() - 1;
  setNextAnimation(animation(_currentCue));
}

int DisplayControllerClass::cueCount() {
  return _demoMode ? AkiraAnimation::demoCueCount() : EEPROM.read(EEPROM_ADDR_CUE_COUNT);
}

AkiraAnimation* DisplayControllerClass::animation(int index) {
  if (_demoMode) {
    return AkiraAnimation::loadDemoCue(index);
  }
  int addr = EEPROM_ADDR_FIRST_CUE + index * sizeof(AkiraAnimation::Cue);
  return AkiraAnimation::loadFromEeprom(&addr);
}

void DisplayControllerClass::setNextAnimation(AkiraAnimation* animation) {
  Serial.print("Displaying cue #");
  Serial.println(_currentCue + 1);
  
  if (_sourceAnimation) {
    delete _sourceAnimation;
  }
  
  _sourceAnimation = _targetAnimation;
  _targetAnimation = animation;
  
  if (!_targetAnimation) {
    return;
  }
  
  if (transition()) {
    transition()->reset();
    _sourceAnimation->setMask(transition()->mask(), Animation::PRE_BLEND);
    _targetAnimation->setMask(transition()->mask(), Animation::OVERLAY);
  }
  else {
    _targetAnimation->setMask(0, Animation::OVERLAY);
  }
}

void DisplayControllerClass::update() {
  if (!_targetAnimation) {
    return;
  }
  
  if (transition() && !transition()->isCompleted()) {
    _targetAnimation->transition()->advance();
    _sourceAnimation->draw();
    _targetAnimation->draw();

    if (transition()->isCompleted()) {
      _sourceAnimation->setMask(0, Animation::PRE_BLEND);
      _targetAnimation->setMask(0, Animation::OVERLAY);
    }
  }
  else {
    _targetAnimation->draw();
  }
}

DisplayControllerClass Display;
