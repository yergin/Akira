#pragma once

#include "Animations.h"

class DisplayController
{
public:
  DisplayController() {}

  void setNextAnimation(AkiraAnimation* animation);
  void update();

private:
  Transition* transition() const { return _targetAnimation && _sourceAnimation ? _targetAnimation->transition() : 0; }

  AkiraAnimation* _targetAnimation = 0;
  AkiraAnimation* _sourceAnimation = 0;
};

