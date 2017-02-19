#include "DisplayController.h"

void DisplayController::setNextAnimation(AkiraAnimation* animation) {
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

void DisplayController::update() {
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

