#include <SPI.h>

#include "Config.h"
#include "Animations.h"
#include "Cue.h"
#include "DualButtonController.h"
#include "StateTransitionTable.h"
#include "StateController.h"
#include "DisplayController.h"

DualButtons::Controller buttons(BUTTON_A_PIN, BUTTON_B_PIN);
void respondToButtons() {
  using namespace DualButtons;
  
  buttons.update();
  if (!buttons.triggered()) {
    return;
  }

  for (int i = 0; i < StateController::transitionTableEntryCount(); ++i) {
    const StateTransition* transition = &stateTransitionTable[i];
    if (transition->currentMode != Akira.currentMode() || !buttons.triggered(transition->button, transition->buttonEvent)) {
      continue;
    }

    Akira.setOperatingModeWithCommand(transition->nextMode, transition->command);
    buttons.performRequest(transition->buttonRequest);
    break;
  }
}

  /*
void setModeAndCommand(Mode nextMode, Command command) {
  if (!areLEDsOn()) {
    if (buttons.triggered(BUTTON_A, PRESS) || buttons.triggered(BUTTON_A, PRESS)) {
      turnLEDsOn();
      buttons.reset();
    }
    return;
  }
  
  if (buttons.triggered(BUTTON_A, SINGLE_TAP)) {
    color1 = (color1 + 1) % COLOR_COUNT;
  }
  
  if (buttons.triggered(BUTTON_A, DOUBLE_TAP)) {
    color2 = (color2 + 1) % COLOR_COUNT;
  }
  
  if (buttons.triggered(BUTTON_A, HOLD)) {   
    animationIndex = (animationIndex + 1) % PRESET_ANIM_COUNT;
    display.setNextAnimation(AnimationFactory.create((AnimationPreset)animationIndex));
  }

  if (buttons.triggered(BUTTON_B, HOLD)) {
    turnLEDsOff();
  }
}
  */

void delayForNextFrame() {
  static unsigned long lastMillis = 0;
  unsigned long t = millis();
  if (t - lastMillis > FRAME_TIME) {
    //Serial.println("Frame delayed!");
    delay(1);
  }
  else {
    delay(FRAME_TIME - (t - lastMillis));
  }
  lastMillis = t;  
}

void setup() {
  delay(400);

  pinMode(LED_ENABLE_PIN, OUTPUT);
  pinMode(LED_SENSE_PIN, INPUT_PULLUP);
  
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.print("Setting up...\n");
#endif

  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);

  Akira.initializeModes();
  Akira.setOperatingMode(MODE_OFF);
}

void loop() {
  respondToButtons();

  Display.update();
  FastLED.show();

  delayForNextFrame();
}
