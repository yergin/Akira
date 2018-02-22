#include <SPI.h>

#include "Config.h"
#include "StateController.h"

void setup() {
  delay(400);

#ifdef PIN_DEBUGGING
  pinMode(DEBUG1_PIN, OUTPUT);
  pinMode(DEBUG2_PIN, OUTPUT);
  digitalWrite(DEBUG1_PIN, LOW);
  digitalWrite(DEBUG2_PIN, LOW);
#endif
  
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println("Initialising...");
#endif

#ifndef LED_ALWAYS_ON
  pinMode(LED_ENABLE_PIN, OUTPUT);
#ifdef SUPPORT_BROKEN_LIGHT_STAFF
  pinMode(LED_ENABLE_PIN2, OUTPUT);
#endif
  pinMode(LED_SENSE_PIN, INPUT_PULLUP);
#endif
  //FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);

  //Akira.initialize();
}

void delayForNextFrame() {
  static unsigned long lastMillis = 0;
  unsigned long t = millis();
  if (t - lastMillis >= FRAME_TIME) {
    //FastLED.delay(1);
    delay(1);
  }
  else {
    //FastLED.delay(FRAME_TIME - (t - lastMillis));
    delay(FRAME_TIME - (t - lastMillis));
  }
  lastMillis = t;
}

void loop() {
  //Akira.update();

  delayForNextFrame();
  Serial.println("Hi!");
}
