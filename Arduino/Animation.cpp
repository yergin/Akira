#include "Animation.h"

Animation::Animation(CRGB* ledBuffer, int ledCount) {
  _ledBuffer = ledBuffer;
  _ledCount = ledCount;
}

void Animation::setMask(char* mask, MaskOperation op) {
  _mask = mask;
  _maskOp = op;
}

void Animation::writeLed(int led, CRGB color) {
  if (_mask) {
    // Optimized color scaling by 0-255
    // Source: https://research.swtch.com/divmult
    if (_maskOp == OVERLAY) {
      int prescale = (_mask[led] << 8) + _mask[led];
      _ledBuffer[led].r += (color.r * prescale + 0x101) >> 16;
      _ledBuffer[led].g += (color.g * prescale + 0x101) >> 16;
      _ledBuffer[led].b += (color.b * prescale + 0x101) >> 16;
    }
    else {
      int prescale = ((255 - _mask[led]) << 8) + 255 - _mask[led];
      _ledBuffer[led].r = (color.r * prescale + 0x101) >> 16;
      _ledBuffer[led].g = (color.g * prescale + 0x101) >> 16;
      _ledBuffer[led].b = (color.b * prescale + 0x101) >> 16;      
    }
  }
  else {
    _ledBuffer[led] = color;
  }
}

void Animation::writeColor(CRGB color) {
  for (int i = 0; i < _ledCount; ++i) {
    writeLed(i, color);
  }
}

void Animation::colorScaleSum(CRGB* out, CRGB color, int scale)
{
  // Optimized color scaling by 0-255
  // Source: https://research.swtch.com/divmult
  int prescale = (scale << 8) + scale;
  out->r += (color.r * prescale + 0x101) >> 16;
  out->g += (color.g * prescale + 0x101) >> 16;
  out->b += (color.b * prescale + 0x101) >> 16;
}

