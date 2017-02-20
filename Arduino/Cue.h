#pragma once

#include "Animations.h"

struct Cue {
  unsigned int color1: 4;
  unsigned int color2: 4;
  unsigned int animation : 4;
};

constexpr Cue CUES[] = {
  { PRESET_COL_LIME, PRESET_COL_AQUA, PRESET_ANIM_GRADIENT },
  { PRESET_COL_MAGENTA, PRESET_COL_ORANGE, PRESET_ANIM_THROB },
  { PRESET_COL_ORANGE, PRESET_COL_BLUE, PRESET_ANIM_LONG_CHASE },
  { PRESET_COL_PINK, PRESET_COL_BLACK, PRESET_ANIM_SHORT_CHASE },
  { PRESET_COL_RAINBOW, PRESET_COL_BLACK, PRESET_ANIM_STROBE }
};

