#include "Animations.h"

AnimationFactoryClass AnimationFactory;

const CRGB COLOR[COLOR_COUNT] = {  0x000000, // black
                                   0x5f5f5f, // white
                                   0xff1f1f, // pink
                                   0xff0000, // red
                                   0xff2f00, // orange
                                   0xdf7700, // yellow
                                   0x8fcf00, // lime
                                   0x00ff00, // green
                                   0x00cf6f, // aqua
                                   0x173fbf, // light blue
                                   0x0000ff, // blue
                                   0x3f00ff, // violet
                                   0x8f008f, // magenta
                                   0x000000, // rainbow
                                };


void AkiraAnimation::setColorPreset1(ColorPreset preset) {
  _colorPreset1 = preset;
  colorPresetsChanged();
}

void AkiraAnimation::setColorPreset2(ColorPreset preset) {
  _colorPreset2 = preset;
  colorPresetsChanged();    
}

void AkiraAnimation::draw() {
  draw(_frame);
  _frame = loopLength() ? (_frame + 1) % loopLength() : _frame + 1;
}

void GradientAnimation::draw(unsigned int /*frame*/) {
  for (int i = 0; i < ledCount(); ++i) {
    int alpha = i * 255 / (ledCount() - 1);
    CRGB pixel = 0;
    colorScaleSum(&pixel, color1(), 255 - alpha);
    colorScaleSum(&pixel, (colorPreset2() == PRESET_COL_BLACK) ? color1() : color2(), alpha);
    writeLed(i, pixel);
  }
}

void ThrobAnimation::draw(unsigned int frame) {
  int alpha = (abs(frame - 63) << 2) + 2;
  CRGB pixel = 0;
  colorScaleSum(&pixel, color1(), alpha);
  colorScaleSum(&pixel, color2(), 255 - alpha);    
  writeColor(pixel);
}

void StrobeAnimation::draw(unsigned int frame) {
  if (colorPreset1() == PRESET_COL_RAINBOW) {
    writeColor(frame == 0 ? CRGB::Red : (frame == 1 ? CRGB::Blue : CRGB::Green));
  }
  else if (colorPreset2() == PRESET_COL_BLACK) {
    writeColor(frame == 0 ? color1() : CRGB::Black);
  }
  else {
    writeColor(frame == 0 ? color1() : (frame == 3 ? color2() : CRGB::Black));        
  }
}

void ShortChaseAnimation::draw(unsigned int frame) {
  for (int i = 0; i < ledCount(); ++i) {
    writeLed(i, (i - (frame >> 1) + 20) % 20 < 10 ? color1() : color2());
  }
}

void LongChaseAnimation::draw(unsigned int frame) {
  for (int i = 0; i < ledCount(); ++i) {
    writeLed(i, (i - frame + 120) % 120 < 60 ? color1() : color2());
  }
}
  
AkiraAnimation* AnimationFactoryClass::create(AnimationPreset preset) {
  switch(preset) {
    case PRESET_ANIM_NONE: return new GradientAnimation;
    case PRESET_ANIM_THROB: return new ThrobAnimation;
    case PRESET_ANIM_LONG_CHASE: return new LongChaseAnimation;
    case PRESET_ANIM_SHORT_CHASE: return new ShortChaseAnimation;
    case PRESET_ANIM_STROBE: return new StrobeAnimation;
    default:
      ;//error!
  }
  return new GradientAnimation();
}


