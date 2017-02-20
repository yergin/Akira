#include "CueDisplay.h"
#include "DisplayController.h"
#include <Arduino.h>

void CueDisplayClass::showFirst() {
  showCue(0);
}

void CueDisplayClass::showNext() {
  showCue((currentCue() + 1) % cueCount());
}

void CueDisplayClass::showPrevious() {
  showCue((currentCue() - 1 + cueCount()) % cueCount());
}

void CueDisplayClass::showCue(int cue) {
  if (cue == _currentCue) {
    return;
  }

  Serial.print("Showing cue #");
  Serial.println(cue + 1);
  
  _currentCue = cue;
  AkiraAnimation* animation = AnimationFactory.create(static_cast<AnimationPreset>(CUES[_currentCue].animation));
  animation->setColorPreset1(static_cast<ColorPreset>(CUES[_currentCue].color1));
  animation->setColorPreset2(static_cast<ColorPreset>(CUES[_currentCue].color2));
  Serial.print("Animation = ");
  Serial.println(CUES[_currentCue].animation);
  Serial.print("Color1 = ");
  Serial.println(CUES[_currentCue].color1);
  Serial.print("Color2 = ");
  Serial.println(CUES[_currentCue].color2);
  Display.setNextAnimation(animation);
}

CueDisplayClass CueDisplay;

