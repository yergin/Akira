#include "Transitions.h"

void Fade::update() {
  char v = static_cast<char>(duration() > 0 ? (currentFrame() + 1) * 255 / duration() : 0);
  for (unsigned int i = 0; i < _ledCount; ++i) {
    _mask[i] = v;
  }
}

void QuickSwipe::update() {
  for (unsigned int i = 0; i < _ledCount; ++i) {
    _mask[i] = i <= currentFrame() ? 255 : 0;
  }
}

QuickFade quickFade;
SlowFade slowFade;
QuickSwipe quickSwipe;

