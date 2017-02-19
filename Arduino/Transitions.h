#pragma once

#include "Transition.h"
#include "Config.h"

class AkiraTransition : public FixedDurationTransition
{
public:
  AkiraTransition() : FixedDurationTransition(transitionMask, LED_COUNT) {}
};

class Fade : public AkiraTransition
{
public:
  void update();
  unsigned int duration() const { return 12; }
};

class QuickFade : public Fade
{
public:
  unsigned int duration() const { return 12; }
};


class SlowFade : public Fade
{
public:
  unsigned int duration() const { return 48; }
};


extern QuickFade quickFade;


