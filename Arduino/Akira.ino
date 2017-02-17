#define SERIAL_DEBUG

#include <SD.h>
#include <SPI.h>
#include <FastLED.h>
#include "Animation.h"
#include "ButtonController.h"
#include "StateTransitionTable.h"
#include "DisplayController.h"

const int LED_CLOCK_PIN = 7;
const int LED_DATA_PIN = 8;
const int LED_ENABLE_PIN = 9;
const int LED_SENSE_PIN = 10;
const int LED_COUNT = 72;
const int SD_CHIP_SELECT = BUILTIN_SDCARD;
const int BUTTON_A_PIN = 5;
const int BUTTON_B_PIN = 6;

const int BRIGHTNESS = 255;

const int FPS = 48;
const unsigned long FRAME_TIME = 1000/FPS;

const int COLOR_COUNT = 15;

enum PresetColor {
  COL_BLACK = 0,
  COL_WHITE,
  COL_PINK,
  COL_RED,
  COL_ORANGE,
  COL_YELLOW,
  COL_LIME,
  COL_GREEN,
  COL_AQUA,
  COL_LIGHT_BLUE,
  COL_BLUE,
  COL_LAVENDAR,
  COL_VIOLET,
  COL_MAGENTA,
  COL_RAINBOW
};

enum PresetAnimation {
  ANIMATION_NONE,
  ANIMATION_THROB,
  ANIMATION_LONG_CHASE,
  ANIMATION_SHORT_CHASE,
  ANIMATION_STROBE
};

CRGB COLORS[COLOR_COUNT] = { 0x000000, // black
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
                             0x4f2f9f, // lavendar
                             0x3f00ff, // violet
                             0x8f008f, // magenta
                             0x000000, // rainbow
                           };


CRGB leds[LED_COUNT];
char transitionMask[LED_COUNT];

File dataFile;

Momentary buttonA;
Momentary buttonB;
DisplayController display;

int color1 = 0;
int color2 = 0;
 
Fade quickFade(transitionMask, LED_COUNT);

class Gradient : public Animation
{
public:
  Gradient(CRGB* ledBuffer, int ledCount) : Animation(ledBuffer, ledCount) {}

  void draw() {
    for (int i = 0; i < ledCount(); ++i) {
      int alpha = i * 255 / (ledCount() - 1);
      CRGB color = 0;
      colorScaleSum(&color, COLORS[color1], 255 - alpha);
      colorScaleSum(&color, COLORS[color2 == COL_BLACK ? color1 : color2], alpha);
      writeLed(i, color);
    }
  }

  Transition* transition() const { return &quickFade; }
};

class Throb : public Animation
{
public:
  Throb(CRGB* ledBuffer, int ledCount) : Animation(ledBuffer, ledCount) {}
  
  void draw() {
    static int throbCounter = 0;

    throbCounter = (throbCounter + 1) % 127;
    int alpha = (abs(throbCounter - 63) << 2) + 2;
    CRGB color = 0;
    colorScaleSum(&color, COLORS[color1], alpha);
    colorScaleSum(&color, COLORS[color2], 255 - alpha);    
    writeColor(color);
  }

  Transition* transition() const { return &quickFade; }
};

class Strobe : public Animation
{
public:
  Strobe(CRGB* ledBuffer, int ledCount) : Animation(ledBuffer, ledCount) {}
  
  void draw() {
    static int strobeCounter = 0;
    if (color1 == COL_RAINBOW) {
      strobeCounter = (strobeCounter + 1) % 3;
      writeColor(strobeCounter == 0 ? CRGB::Red : (strobeCounter == 1 ? CRGB::Blue : CRGB::Green));
    }
    else if (color2 == COL_BLACK) {
      strobeCounter = (strobeCounter + 1) % 3;
      writeColor(COLORS[strobeCounter == 0 ? color1 : COL_BLACK]);
    }
    else {
      strobeCounter = (strobeCounter + 1) % 6;
      writeColor(COLORS[strobeCounter == 0 ? color1 : (strobeCounter == 3 ? color2 : COL_BLACK)]);        
    }
  }
};

class ShortChase : public Animation
{
public:
  ShortChase(CRGB* ledBuffer, int ledCount) : Animation(ledBuffer, ledCount) {}
  
  void draw() {
    static int chaseCounter = 0;

    chaseCounter = (chaseCounter + 1) % 40;
    for (int i = 0; i < ledCount(); ++i) {
      writeLed(i, COLORS[(i - (chaseCounter >> 1) + 20) % 20 < 10 ? color1 : color2]);
    }
  }

  Transition* transition() const { return &quickFade; }
};

class LongChase : public Animation
{
public:
  LongChase(CRGB* ledBuffer, int ledCount) : Animation(ledBuffer, ledCount) {}
  
  void draw() {
    static int chaseCounter = 0;

    chaseCounter = (chaseCounter + 1) % 120;
    for (int i = 0; i < ledCount(); ++i) {
      writeLed(i, COLORS[(i - (chaseCounter) + 120) % 120 < 60 ? color1 : color2]);
    }
  }

  Transition* transition() const { return &quickFade; }
};


Gradient gradient(leds, LED_COUNT);
Throb throb(leds, LED_COUNT);
LongChase longChase(leds, LED_COUNT);
ShortChase shortChase(leds, LED_COUNT);
Strobe strobe(leds, LED_COUNT);

const int ANIMATION_COUNT = 5;
Animation* animations[ANIMATION_COUNT] = {&gradient, &throb, &longChase, &shortChase, &strobe};
int animationIndex = 0;

void respondToButtons() {
  buttonA.update();
  buttonB.update();

  if (!areLEDsOn()) {
    if (buttonA.triggered(Momentary::PRESS)) {
      turnLEDsOn();
      buttonA.reset();
    }
    return;
  }
  
  if (buttonA.triggered(Momentary::SINGLE_TAP)) {
    color1 = (color1 + 1) % COLOR_COUNT;
  }
  
  if (buttonA.triggered(Momentary::DOUBLE_TAP)) {
    color2 = (color2 + 1) % COLOR_COUNT;
  }
  
  if (buttonA.triggered(Momentary::HOLD)) {
    
    animationIndex = (animationIndex + 1) % ANIMATION_COUNT;
    display.setNextAnimation(animations[animationIndex]);
    
    //turnLEDsOff();
  }
}

void delayForNextFrame() {
  static unsigned long lastMillis = 0;
  unsigned long t = millis();
  if (t - lastMillis > FRAME_TIME) {
    Serial.println("Frame delayed!");
    delay(1);
  }
  else {
    delay(FRAME_TIME - (t - lastMillis));
  }
  lastMillis = t;  
}

bool areLEDsOn() {
  return digitalRead(LED_SENSE_PIN) == LOW;
}

void turnLEDsOff() {
  pinMode(LED_CLOCK_PIN, INPUT_PULLUP);
  pinMode(LED_DATA_PIN, INPUT_PULLUP);
  digitalWrite(LED_ENABLE_PIN, LOW);
}

void turnLEDsOn() {
  digitalWrite(LED_ENABLE_PIN, HIGH);
  delay(1);
  if (!areLEDsOn()) {
    Serial.print("Attempted to enable LEDs but none detected.\n");
    return;
  }
  pinMode(LED_CLOCK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);
}

void setup() {
  delay(400);

  buttonA.attach(BUTTON_A_PIN, INPUT_PULLUP);
  buttonB.attach(BUTTON_B_PIN, INPUT_PULLUP);

  turnLEDsOff();
  pinMode(LED_ENABLE_PIN, OUTPUT);
  pinMode(LED_SENSE_PIN, INPUT_PULLUP);
  
  #ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.print("Setting up...\n");
  #endif

  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.print("SD card could not be read.\n");
  }
  else {
    Serial.print("SD card could be read.\n");
  }

  turnLEDsOn();
  if (areLEDsOn()) {
    FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);
  }
  
  FastLED.setBrightness(BRIGHTNESS);
  
  FastLED.showColor(COLORS[COL_WHITE]);
  delay(400);  
  FastLED.showColor(COLORS[COL_BLACK]);

  display.setNextAnimation(animations[animationIndex]);
}

void loop() {
  respondToButtons();

  display.update();
  FastLED.show();

  delayForNextFrame();
}
