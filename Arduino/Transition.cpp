#include "Transition.h"

Transition::Transition(char* mask, int ledCount)
  : _ledCount(ledCount), _mask(mask)
{}

void FixedDurationTransition::setDuration(int frameCount) {
  _currentFrame = _currentFrame * frameCount / _frameCount;
  _frameCount = frameCount;
}

void FixedDurationTransition::reset() {
  _currentFrame = 0;
}

void FixedDurationTransition::advance() {
  if (isCompleted()) {
    return;
  }
  update();
  _currentFrame++;
}

void Fade::update() {
  char v = static_cast<char>(frameCount() > 0 ? (currentFrame() + 1) * 255 / frameCount() : 0);
  for (int i = 0; i < _ledCount; ++i) {
    _mask[i] = v;
  }
}

