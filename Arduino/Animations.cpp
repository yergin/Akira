#include "Animations.h"
#include <EEPROM.h>

constexpr AkiraAnimation::CueDescription AkiraAnimation::_demo[];

const uint8_t waveLookup[256] = { 0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,4,4,5,5,6,6,6,7,8,8,9,9,10,10,11,12,12,13,14,14,15,16,17,17,18,19,20,21,22,23,23,24,25,26,27,28,29,30,31,32,33,34,35,37,38,39,40,41,42,43,45,46,47,48,49,51,52,53,54,56,57,58,60,61,62,64,65,66,68,69,71,72,73,75,76,78,79,81,82,84,85,87,88,90,91,93,94,96,97,99,100,102,103,105,106,108,109,111,113,114,116,117,119,120,122,124,125,127,128,130,131,133,135,136,138,139,141,142,144,146,147,149,150,152,153,155,156,158,159,161,162,164,165,167,168,170,171,173,174,176,177,179,180,182,183,184,186,187,189,190,191,193,194,195,197,198,199,201,202,203,204,206,207,208,209,210,212,213,214,215,216,217,218,220,221,222,223,224,225,226,227,228,229,230,231,232,232,233,234,235,236,237,238,238,239,240,241,241,242,243,243,244,245,245,246,246,247,247,248,249,249,249,250,250,251,251,252,252,252,253,253,253,253,254,254,254,254,254,255,255,255,255,255,255,255,255 };

const unsigned int MAX_HUE = 2560;

CRGB colorFromHue(unsigned int hue) {
  static const int firstCol = PRESET_COL_RED;
  static const int lastCol = PRESET_COL_MAGENTA;
  static const int numColors = lastCol - firstCol + 1;
  CRGB col1 = COLOR[(hue >> 8) % numColors + firstCol];
  CRGB col2 = COLOR[((hue >> 8) + 1) % numColors + firstCol];
  int alpha = hue % 256;
  CRGB pixel = 0;
  Animation::colorScaleSum(&pixel, col1, 255 - alpha);
  Animation::colorScaleSum(&pixel, col2, alpha);
  return pixel;
}

AkiraAnimation* AkiraAnimation::create(AnimationPreset preset) {
  AkiraAnimation* animation = 0;
  switch(preset) {
    case PRESET_ANIM_OFF: animation = new GradientAnimation; break;
    case PRESET_ANIM_GRADIENT: animation = new GradientAnimation; break;
    case PRESET_ANIM_THROB: animation = new ThrobAnimation; break;
    case PRESET_ANIM_WAVE: animation = new WaveAnimation; break;
    case PRESET_ANIM_SHORT_CHASE: animation = new ShortChaseAnimation; break;
    case PRESET_ANIM_LONG_CHASE: animation = new LongChaseAnimation; break;
    case PRESET_ANIM_STROBE: animation = new StrobeAnimation; break;
    case PRESET_ANIM_SPARKLE: animation = new SparkleAnimation; break;
    case PRESET_ANIM_FLICKER: animation = new FlickerAnimation; break;
    case PRESET_ANIM_SPLIT: animation = new SplitAnimation; break;
    default: animation = new GradientAnimation; break;
  }

  animation->_cue.descr.animation = preset;
  return animation;
}

AkiraAnimation* AkiraAnimation::create(const CueDescription& descr) {
  AkiraAnimation* animation = AkiraAnimation::create(static_cast<AnimationPreset>(descr.animation));
  animation->setColorPreset1(static_cast<ColorPreset>(descr.color1));
  animation->setColorPreset2(static_cast<ColorPreset>(descr.color2));
  return animation;
}

AkiraAnimation* AkiraAnimation::loadDemoCue(int index) {
  return AkiraAnimation::create(_demo[index]);
}

AkiraAnimation* AkiraAnimation::loadFromEeprom(int *addr) {
  Cue cue;
  cue.data[0] = EEPROM.read(*addr);
  cue.data[1] = EEPROM.read(*addr + 1);
  *addr += EEPROM_CUE_SIZE;

  if (cue.descr.animation > PRESET_ANIM_COUNT) {
    cue.descr.animation = PRESET_ANIM_GRADIENT;
  }

  if (cue.descr.color1 > PRESET_COL_COUNT) {
    cue.descr.color1 = PRESET_COL_WHITE;
  }

  if (cue.descr.color2 > PRESET_COL_COUNT) {
    cue.descr.color2 = PRESET_COL_RAINBOW_OR_BLACK;
  }

  return AkiraAnimation::create(cue.descr);
}

void AkiraAnimation::saveToEeprom(int *addr) {
  EEPROM.write(*addr, _cue.data[0]);
  EEPROM.write(*addr + 1, _cue.data[1]);
  *addr += EEPROM_CUE_SIZE;
}

void AkiraAnimation::copyToDescription(CueDescription* descr) {
  descr->animation = _cue.descr.animation;
  descr->color1 = _cue.descr.color1;
  descr->color2 = _cue.descr.color2;
}

void AkiraAnimation::setColorPreset1(ColorPreset preset) {
  _cue.descr.color1 = preset;
  colorPresetsChanged();
}

void AkiraAnimation::setColorPreset2(ColorPreset preset) {
  _cue.descr.color2 = preset;
  colorPresetsChanged();    
}

void AkiraAnimation::draw() {
  draw(_frame);
  _frame = loopLength() ? (_frame + 1) % loopLength() : _frame + 1;
}

void GradientAnimation::draw(unsigned int /*frame*/) {
  CRGB col2 = (colorPreset2() == PRESET_COL_RAINBOW_OR_BLACK) ? color1() : color2();
  for (int i = 0; i < ledCount(); ++i) {
    int alpha = waveLookup[i * 255 / (ledCount() - 1)];
    CRGB pixel = 0;
    colorScaleSum(&pixel, color1(), 255 - alpha);
    colorScaleSum(&pixel, col2, alpha);
    writeLed(i, pixel);
  }
}

void SplitAnimation::draw(unsigned int /*frame*/) {
  CRGB col2 = (colorPreset2() == PRESET_COL_RAINBOW_OR_BLACK) ? color1() : color2();
  for (int i = 0; i < ledCount(); ++i) {
    writeLed(i, i < ledCount() / 2 ? color1() : col2);
  }
}

void ThrobAnimation::draw(unsigned int frame) {
  if (colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK) {
    writeColor(colorFromHue(frame * MAX_HUE / loopLength()));
    return;
  }
  
  int v = -63 + frame;
  int alpha = waveLookup[(abs(v) << 2) + 2];
  CRGB pixel = 0;
  colorScaleSum(&pixel, color1(), alpha);
  colorScaleSum(&pixel, color2(), 255 - alpha);    
  writeColor(pixel);
}

void WaveAnimation::draw(unsigned int frame) {
  if (colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK) {
    for (int i = 0; i < ledCount(); ++i) {
      unsigned int hue = ((i << 5) - (frame << 5) + 0xFFFF0000) % 2560;
      writeLed(i, colorFromHue(hue));
    }
    return;
  }
  
  CRGB col2 = (colorPreset2() == PRESET_COL_RAINBOW_OR_BLACK) ? color1() : color2();
  for (int i = 0; i < ledCount(); ++i) {
    unsigned int index = ((i << 4) - (frame << 3) + 0xFFFF0000) % 510;
    index = index < 256 ? index : 510 - index;
    int alpha = waveLookup[index];
    CRGB pixel = 0;
    colorScaleSum(&pixel, color1(), 255 - alpha);
    colorScaleSum(&pixel, col2, alpha);
    writeLed(i, pixel);
  }
}

void StrobeAnimation::draw(unsigned int frame) {
  if (colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK) {
    writeColor(frame == 0 ? CRGB::Red : (frame == 1 ? CRGB::Blue : CRGB::Green));
  }
  else if (colorPreset2() == PRESET_COL_RAINBOW_OR_BLACK) {
    writeColor(frame == 0 ? color1() : CRGB::Black);
  }
  else {
    writeColor(frame == 0 ? color1() : (frame == 3 ? color2() : CRGB::Black));        
  }
}

void LongChaseAnimation::draw(unsigned int frame) {
  for (int i = 0; i < ledCount(); ++i) {
    writeLed(i, (i - frame + 120) % 120 < 60 ? color1() : color2());
  }
}

void ShortChaseAnimation::draw(unsigned int frame) {
  if (colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK) {
    for (int i = 0; i < ledCount(); ++i) {
      writeLed(i, colorFromHue((((i - (frame >> 1) + 100) % 200) / 20) << 9));
    }
    return;
  }
  for (int i = 0; i < ledCount(); ++i) {
    writeLed(i, (i - (frame >> 1) + 20) % 20 < 10 ? color1() : color2());
  }
}

SparkleAnimation::Spark* SparkleAnimation::nextSpark() {
  for (int i = 0; i < SPARK_COUNT; ++i) {
    if (_sparks[i].brightness < 0) {
      return &_sparks[i];
    }
  }
  return 0;
}

void SparkleAnimation::draw(unsigned int /*frame*/) {
  if (_sparkCountdown <= 0) {
    Spark* spark = 0;
    for (int i = 0; i < SPARK_MULTIPLIER; i++) {
      spark = nextSpark();
      if (spark) {
        spark->pos = random(LED_COUNT);
        spark->brightness = 255;
        if (colorPreset1() == PRESET_COL_RAINBOW_OR_BLACK) {
          spark->color = colorFromHue(random(MAX_HUE));//;COLOR[random(PRESET_COL_RED, PRESET_COL_RAINBOW_OR_BLACK)];
        }
        else if (colorPreset2() == PRESET_COL_RAINBOW_OR_BLACK) {
          spark->color = color1();
        }
        else {
          spark->color = random(2) ? color1() : color2();
        }
      }
      _sparkCountdown = SPARK_INTERVAL;
    }
  }

  for (int i = 0; i < SPARK_COUNT; ++i) {
    if (_sparks[i].brightness >= 0) {
      CRGB pixel = 0;
      colorScaleSum(&pixel, _sparks[i].color, _sparks[i].brightness);
      writeLed(_sparks[i].pos, pixel);
      if (_sparks[i].brightness) {
        _sparks[i].brightness = (_sparks[i].brightness >> 1) + (_sparks[i].brightness >> 2);
      }
      else {
        _sparks[i].brightness = -1;
      }
    }
  }  
  
  _sparkCountdown--;
}

void FlickerAnimation::draw(unsigned int /*frame*/) {
  
}


