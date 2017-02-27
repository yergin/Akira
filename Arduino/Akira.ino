#include <SPI.h>

#include "Config.h"
#include "StateController.h"

void setup() {
  delay(400);

#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println("Initialising...");
#endif

  pinMode(LED_ENABLE_PIN, OUTPUT);
  pinMode(LED_SENSE_PIN, INPUT_PULLUP);
  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);

  Akira.initialize();
}

void delayForNextFrame() {
  static unsigned long lastMillis = 0;
  unsigned long t = millis();
  if (t - lastMillis > FRAME_TIME) {
    FastLED.delay(1);
  }
  else {
    FastLED.delay(FRAME_TIME - (t - lastMillis));
  }
  lastMillis = t;  
}

void loop() {
  Akira.update();

  delayForNextFrame();
}
