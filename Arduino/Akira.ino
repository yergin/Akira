#include <SPI.h>

#include "Config.h"
#include "StateController.h"

void setup() {
  delay(400);

  pinMode(LED_ENABLE_PIN, OUTPUT);
  pinMode(LED_SENSE_PIN, INPUT_PULLUP);
  
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println("Initialising...");
#endif

  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(leds, LED_COUNT);

  Akira.initialize();

  unsigned int sum = 0;
  for (int i = 0; i < 16; ++i) {
    delay(20);
    sum += analogRead(BATT_VOLTAGE_PIN);
  }
  //Serial.print("Battery voltage: ");
  Serial.println(sum * BATT_VOLTAGE_SCALER >> 14);
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
  Akira.update();

  delayForNextFrame();
}
