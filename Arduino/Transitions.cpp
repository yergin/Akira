#include "Transitions.h"

QuickFade quickFade;

void Fade::update() {
  char v = static_cast<char>(duration() > 0 ? (currentFrame() + 1) * 255 / duration() : 0);
  for (int i = 0; i < _ledCount; ++i) {
    _mask[i] = v;
  }
}

