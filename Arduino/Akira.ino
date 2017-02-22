#include <SPI.h>

#include "Config.h"
#include "Animations.h"
#include "DualButtonController.h"
#include "StateTransitionTable.h"
#include "StateController.h"

void setup() {
  delay(400);

  pinMode(LED_ENABLE_PIN, OUTPUT);
  pinMode(LED_SENSE_PIN, INPUT_PULLUP);
  
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println("Setting up...");
#endif

  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);
  
  Akira.initialize();
  Serial.println("Finished setting up.");
}

void respondToButtons() {
  // TODO: move to StateController
  using namespace DualButtons;
  
  Buttons.update();
  if (!Buttons.triggered()) {
    return;
  }

  for (int i = 0; i < StateController::transitionTableEntryCount(); ++i) {
    const StateTransition* transition = &stateTransitionTable[i];
    if (transition->currentMode != Akira.currentMode() || !Buttons.triggered(transition->button, transition->buttonEvent)) {
      continue;
    }

    Akira.setOperatingModeWithCommand(transition->nextMode, transition->command);
    Buttons.performRequest(transition->buttonRequest);
    break;
  }
}

void delayForNextFrame() {
  static unsigned long lastMillis = 0;
  unsigned long t = millis();
  if (t - lastMillis > FRAME_TIME) {
    //Serial.println("Frame delayed!");
    FastLED.delay(1);
  }
  else {
    FastLED.delay(FRAME_TIME - (t - lastMillis));
  }
  lastMillis = t;  
}

void loop() {
  respondToButtons();

  Akira.update();
  FastLED.show();

  delayForNextFrame();
}
