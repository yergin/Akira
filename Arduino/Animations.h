#pragma once

#include "Animation.h"
#include "Config.h"
#include "Transitions.h"

enum AnimationPreset {
  PRESET_ANIM_OFF,
  PRESET_ANIM_GRADIENT,
  PRESET_ANIM_THROB,
  PRESET_ANIM_WAVE,
  PRESET_ANIM_SHORT_CHASE,
  PRESET_ANIM_LONG_CHASE,
  PRESET_ANIM_STROBE,
  PRESET_ANIM_SPARKLE,
  PRESET_ANIM_FLICKER,
  PRESET_ANIM_SPLIT,
  PRESET_ANIM_COUNT
};

enum ColorPreset {
  PRESET_COL_WHITE = 0,
  PRESET_COL_PINK,
  PRESET_COL_RED,
  PRESET_COL_ORANGE,
  PRESET_COL_YELLOW,
  PRESET_COL_LIME,
  PRESET_COL_GREEN,
  PRESET_COL_AQUA,
  PRESET_COL_BLUE,
  PRESET_COL_VIOLET,
  PRESET_COL_FUCHSIA,
  PRESET_COL_MAGENTA,
  PRESET_COL_RAINBOW_OR_BLACK,
  PRESET_COL_COUNT
};

constexpr int COLOR[] = { 0x5f5f5f, // white
                          0xff1f1f, // pink
                          0xff0000, // red
                          0xff2f00, // orange
                          0xdf7700, // yellow
                          0x8fcf00, // lime
                          0x00ff00, // green
                          0x00cf6f, // aqua
                          0x0000ff, // blue
                          0x3f00ff, // violet
                          0x8f008f, // fuchsia
                          0xcf005f, // magenta
                          0x000000, // rainbow / black
                        };

class AkiraAnimation : public Animation
{
public:
  struct CueDescription {
    ColorPreset color1: 4;
    ColorPreset color2: 4;
    unsigned int animation : 4;
  };
  
  union Cue {
    CueDescription descr;
    uint8_t data[EEPROM_CUE_SIZE];
  };
  
  static AkiraAnimation* create(AnimationPreset preset);
  static AkiraAnimation* create(const CueDescription& descr);
  static AkiraAnimation* loadFromEeprom(int* addr);
  static AkiraAnimation* loadDemoCue(int index);
  static int demoCueCount() { return sizeof(_demo) / sizeof(_demo[0]); }
  
  AkiraAnimation() : Animation(leds, LED_COUNT) {}
  virtual ~AkiraAnimation() {}

  void saveToEeprom(int* addr);
  void copyToDescription(CueDescription* descr);
  
  virtual Transition* transition() const { return 0; }

  virtual void setColorPreset1(ColorPreset preset);
  virtual void setColorPreset2(ColorPreset preset);

  ColorPreset colorPreset1() const { return _cue.descr.color1; }
  ColorPreset colorPreset2() const { return _cue.descr.color2; }
  CRGB color1() const { return COLOR[colorPreset1()]; }
  CRGB color2() const { return COLOR[colorPreset2()]; }

  void draw();

  virtual void reset() { _frame = 0; }
  
protected:
  virtual void draw(unsigned int frame) = 0;
  virtual unsigned int loopLength() const { return 0; }
  virtual void colorPresetsChanged() {}

private:
  static constexpr CueDescription _demo[] = {
    { PRESET_COL_LIME, PRESET_COL_AQUA, PRESET_ANIM_GRADIENT },
    { PRESET_COL_MAGENTA, PRESET_COL_ORANGE, PRESET_ANIM_THROB },
    { PRESET_COL_ORANGE, PRESET_COL_BLUE, PRESET_ANIM_LONG_CHASE },
    { PRESET_COL_PINK, PRESET_COL_RAINBOW_OR_BLACK, PRESET_ANIM_SHORT_CHASE },
    { PRESET_COL_RAINBOW_OR_BLACK, PRESET_COL_RAINBOW_OR_BLACK, PRESET_ANIM_STROBE }
  };

  Cue _cue;
  unsigned int _frame = 0;
};

class OffAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &quickFade; }

protected:
  void draw(unsigned int frame);
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
  unsigned int loopLength() const { return colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK ? 62 * 10 : 126; } 
  void colorPresetsChanged() { reset(); }
};

class WaveAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &slowFade; }

protected:
  void draw(unsigned int frame);
};

class ShortChaseAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &slowFade; }

protected:
  void draw(unsigned int frame);
  unsigned int loopLength() const { return colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK ? 200 : 40; }
  void colorPresetsChanged() { reset(); }
};

class LongChaseAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &quickSwipe; }
  
protected:
  void draw(unsigned int frame);
  unsigned int loopLength() const { return 120; }
};

class StrobeAnimation : public AkiraAnimation
{
protected:
  void draw(unsigned int frame);
  
  unsigned int loopLength() const {
    return (colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK || colorPreset2() == PRESET_COL_RAINBOW_OR_BLACK) ? 3 : 6;
  }

  void colorPresetsChanged() { reset(); }
};

class SparkleAnimation : public AkiraAnimation
{
protected:
  static constexpr int SPARK_COUNT = 1000;
  static constexpr int SPARK_INTERVAL = 1;
  
  struct Spark {
    int pos = 0;
    int brightness = -1;
  };

  void draw(unsigned int frame);
  Spark* nextSpark();

  Spark _sparks[SPARK_COUNT];
  int _sparkCountdown = 0;
};

class FlickerAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &slowFade; }

protected:
  void draw(unsigned int frame);
};

class SplitAnimation : public AkiraAnimation
{
public:
  Transition* transition() const { return &quickFade; }

protected:
  void draw(unsigned int frame);
};


