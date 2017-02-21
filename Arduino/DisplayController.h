#pragma once

#include "Animations.h"

class DisplayControllerClass
{
public:
  DisplayControllerClass() {}

  void showFirstCue();
  void showNextCue();
  void showPreviousCue();

  void update();

private:
  void setNextAnimation(AkiraAnimation* animation);
  int cueCount();
  AkiraAnimation* animation(int index);
  Transition* transition() const { return _targetAnimation && _sourceAnimation ? _targetAnimation->transition() : 0; }

  AkiraAnimation* _targetAnimation = 0;
  AkiraAnimation* _sourceAnimation = 0;
  int _currentCue = -1;
  bool _demoMode = true;
};

extern DisplayControllerClass Display;
