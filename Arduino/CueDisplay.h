#pragma once

#include "Cue.h"

class CueDisplayClass {
public:
  void showFirst();
  void showNext();
  void showPrevious();

private:
  void showCue(int cue);
  int currentCue() const { return _currentCue; }
  int cueCount() const { return sizeof(CUES) / sizeof(CUES[0]); }
  
  int _currentCue = -1;
};

extern CueDisplayClass CueDisplay;
