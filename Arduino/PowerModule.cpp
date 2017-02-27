#include "PowerModule.h"
#include "DualButtonController.h"
#include <Arduino.h>
#include <Snooze.h>

SnoozeDigital Digital;
SnoozeBlock SnoozeConfig(Digital);

void PowerModule::initialize() {
  Digital.pinMode(BUTTON_A_PIN, INPUT_PULLUP, FALLING);
  Digital.pinMode(BUTTON_B_PIN, INPUT_PULLUP, FALLING);
}

void PowerModule::update() {
  if (_frame++ % BATT_VOLTAGE_FILT_DOWNSAMPLE) {
     return;
  }

  int milliVolts = (analogRead(BATT_VOLTAGE_PIN) * BATT_MILLIVOLT_MAX) >> 10;
  _milliVolts = _milliVolts ? (_milliVolts * ((1 << BATT_VOLTAGE_FILT_EXP) - 1) + milliVolts) >> BATT_VOLTAGE_FILT_EXP : milliVolts;
  if (_milliVolts < _criticalThreshold) {
    setBatteryState(BATTERY_CRITICAL);
  }
  else if (_milliVolts < _lowThreshold) {
    setBatteryState(BATTERY_CRITICAL);  
  }
  else {
    setBatteryState(BATTERY_OK);
  }
}

void PowerModule::setBatteryState(BatteryState state) {
  if (_batteryState == state) {
    return;
  }

  _batteryState = state;
  switch (_batteryState) {
    case BATTERY_OK:
      _lowThreshold = BATT_MILLIVOLT_LOW;
      _criticalThreshold = BATT_MILLIVOLT_CRITICAL;
      break;
      
    case BATTERY_LOW:
      _lowThreshold = BATT_MILLIVOLT_LOW + BATT_MILLIVOLT_HYSTERESIS;
      _criticalThreshold = BATT_MILLIVOLT_CRITICAL;
      break;
      
    case BATTERY_CRITICAL:
      _lowThreshold = BATT_MILLIVOLT_LOW + BATT_MILLIVOLT_HYSTERESIS;
      _criticalThreshold = BATT_MILLIVOLT_CRITICAL + BATT_MILLIVOLT_HYSTERESIS;
      break;
  }
}

bool PowerModule::areLedsOn() const {
  return digitalRead(LED_SENSE_PIN) == LOW;
}

void PowerModule::turnLedsOn() {
#ifdef SERIAL_DEBUG
  Serial.println("Turning LEDs on.");
#endif
  digitalWrite(LED_ENABLE_PIN, HIGH);
  delay(1);
  if (!areLedsOn()) {
#ifdef SERIAL_DEBUG
    Serial.println("Attempted to power up LEDs but no voltage detected.");
#endif
    return;
  }
  pinMode(LED_CLOCK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);    
}

void PowerModule::turnLedsOff() {
#ifdef SERIAL_DEBUG
  Serial.println("Turning LEDs off.");
#endif
  pinMode(LED_CLOCK_PIN, INPUT);
  pinMode(LED_DATA_PIN, INPUT);
  digitalWrite(LED_ENABLE_PIN, LOW);
  delay(1);
  if (areLedsOn()) {
#ifdef SERIAL_DEBUG
    Serial.println("Failed to power down LEDs.");
#endif
    FastLED.showColor(CRGB::Black);
    return;
  }  
}

void PowerModule::deepSleep() {
  bool ledsOn = areLedsOn();
  if (ledsOn) {
    turnLedsOff();
  }
  Buttons.sleep();
  FastLED.showColor(CRGB::Black);
  FastLED.delay(5);
  Snooze.deepSleep(SnoozeConfig);
  Buttons.wakeup();
  if (ledsOn) {
    turnLedsOn();
  }
}

void PowerModule::setBrightness(uint8_t brightness) {
  _brightness = brightness;
  if (_lowPowerMode) {
    FastLED.setBrightness(_brightness < LOW_POWER_MAX_BRIGHTNESS ? _brightness : LOW_POWER_MAX_BRIGHTNESS);
    return;
  }
  
  FastLED.setBrightness(_brightness);
}

void PowerModule::setLowPowerMode(bool enable) {
  if (enable == _lowPowerMode) {
    return;
  }
#ifdef SERIAL_DEBUG
  Serial.print("Low-Power mode ");
  Serial.println(enable ? "enabled." : "disabled.");
#endif  
  _lowPowerMode = enable;
  setBrightness(_brightness);
}

PowerModule Power;
