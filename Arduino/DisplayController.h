#pragma once

#include "Animation.h"

class DisplayController
{
public:
  DisplayController() {}

  void setNextAnimation(Animation* animation);
  void update();

private:
  Transition* transition() const { return _targetAnimation && _sourceAnimation ? _targetAnimation->transition() : 0; }

  Animation* _targetAnimation = 0;
  Animation* _sourceAnimation = 0;
};

