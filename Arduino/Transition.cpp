#include "Transition.h"

Transition::Transition(char* mask, int ledCount)
  : _ledCount(ledCount), _mask(mask)
{}

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

