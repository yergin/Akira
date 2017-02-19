#define SERIAL_DEBUG

#include <SPI.h>

#include "Config.h"
#include "Animations.h"
#include "Cue.h"
#include "ButtonController.h"
#include "StateTransitionTable.h"
#include "DisplayController.h"

Momentary buttonA;
Momentary buttonB;
DisplayController display;

int color1 = 0;
int color2 = 0;

int animationIndex = 0;

void respondToButtons() {
  buttonA.update();
  buttonB.update();

  if (!areLEDsOn()) {
    if (buttonA.triggered(Momentary::PRESS) || buttonB.triggered(Momentary::PRESS)) {
      turnLEDsOn();
      buttonA.reset();
      buttonB.reset();
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
    animationIndex = (animationIndex + 1) % PRESET_ANIM_COUNT;
    display.setNextAnimation(AnimationFactory.create((AnimationPreset)animationIndex));// animations[animationIndex]);
  }

  if (buttonB.triggered(Momentary::HOLD)) {
    turnLEDsOff();
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
  pinMode(LED_CLOCK_PIN, INPUT);
  pinMode(LED_DATA_PIN, INPUT);
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

  turnLEDsOn();
  if (areLEDsOn()) {
    FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);
  }
  
  FastLED.setBrightness(BRIGHTNESS);
  
  FastLED.showColor(COLOR[PRESET_COL_WHITE]);
  delay(400);  
  FastLED.showColor(COLOR[PRESET_COL_BLACK]);

  display.setNextAnimation(AnimationFactory.create((AnimationPreset)animationIndex));
}

void loop() {
  respondToButtons();

  display.update();
  FastLED.show();

  delayForNextFrame();
}
