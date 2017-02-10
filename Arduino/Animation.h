#pragma once

#include <FastLED.h>
#include "Transition.h"

class Animation
{
public:
  enum MaskOperation {
    OVERLAY = 0,
    PRE_BLEND
  };

  static void colorScaleSum(CRGB* out, CRGB col, int scale);
  
  Animation(CRGB* ledBuffer, int ledCount);

  void setMask(char* mask, MaskOperation op);
  virtual void draw() = 0;
  virtual Transition* transition() const { return 0; }

  int ledCount() const { return _ledCount; }

protected:
  void writeLed(int led, CRGB color);
  void writeColor(CRGB color);

private:
  int _ledCount;
  CRGB* _ledBuffer;
  char* _mask = 0;
  MaskOperation _maskOp = OVERLAY;
};

