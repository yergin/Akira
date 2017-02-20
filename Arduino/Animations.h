#pragma once

#include "Animation.h"
#include "Config.h"
#include "Transitions.h"

constexpr int COLOR_COUNT = 14;
extern const CRGB COLOR[COLOR_COUNT];

enum ColorPreset {
  PRESET_COL_BLACK = 0,
  PRESET_COL_WHITE,
  PRESET_COL_PINK,
  PRESET_COL_RED,
  PRESET_COL_ORANGE,
  PRESET_COL_YELLOW,
  PRESET_COL_LIME,
  PRESET_COL_GREEN,
  PRESET_COL_AQUA,
  PRESET_COL_LIGHT_BLUE,
  PRESET_COL_BLUE,
  PRESET_COL_VIOLET,
  PRESET_COL_MAGENTA,
  PRESET_COL_RAINBOW,
  PRESET_COL_COUNT
};

enum AnimationPreset {
  PRESET_ANIM_GRADIENT,
  PRESET_ANIM_THROB,
  PRESET_ANIM_LONG_CHASE,
  PRESET_ANIM_SHORT_CHASE,
  PRESET_ANIM_STROBE,
  PRESET_ANIM_COUNT
};

class AkiraAnimation : public Animation
{
public:
  AkiraAnimation() : Animation(leds, LED_COUNT) {}
  virtual ~AkiraAnimation() {}

  virtual Transition* transition() const { return 0; }

  virtual void setColorPreset1(ColorPreset preset);
  virtual void setColorPreset2(ColorPreset preset);

  ColorPreset colorPreset1() const { return _colorPreset1; }
  ColorPreset colorPreset2() const { return _colorPreset2; }
  CRGB color1() const { return COLOR[_colorPreset1]; }
  CRGB color2() const { return COLOR[_colorPreset2]; }

  void draw();

  virtual void reset() { _frame = 0; }
  
protected:
  virtual void draw(unsigned int frame) = 0;
  virtual unsigned int loopLength() const { return 0; }
  virtual void colorPresetsChanged() {}

private:
  ColorPreset _colorPreset1 = PRESET_COL_WHITE;
  ColorPreset _colorPreset2 = PRESET_COL_BLACK;
  unsigned int _frame = 0;
};

class GradientAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &quickFade; }

protected:
  void draw(unsigned int frame);
};

class ThrobAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &slowFade; }

protected:
  void draw(unsigned int frame);
  unsigned int loopLength() const { return 126; } 
};

class StrobeAnimation : public AkiraAnimation
{
protected:
  void draw(unsigned int frame);
  
  unsigned int loopLength() const {
    return (colorPreset1() == PRESET_COL_RAINBOW || colorPreset2() == PRESET_COL_BLACK) ? 3 : 6;
  }

  void colorPresetsChanged() { reset(); }
};

class ShortChaseAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &slowFade; }

protected:
  void draw(unsigned int frame);
  unsigned int loopLength() const { return 40; }
};

class LongChaseAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &quickSwipe; }
  
protected:
  void draw(unsigned int frame);
  unsigned int loopLength() const { return 120; }
};

class AnimationFactoryClass {
public:
  AkiraAnimation* create(AnimationPreset preset);
};

extern AnimationFactoryClass AnimationFactory;
