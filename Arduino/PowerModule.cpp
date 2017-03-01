#include "PowerModule.h"
#include "DualButtonController.h"
#include <Arduino.h>
#include <Snooze.h>

SnoozeDigital Digital;
SnoozeBlock SnoozeConfig(Digital);

void PowerModule::initialize() {
  Digital.pinMode(BUTTON_A_PIN, INPUT_PULLUP, FALLING);
  Digital.pinMode(BUTTON_B_PIN, INPUT_PULLUP, FALLING);
  analogReference(INTERNAL);
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
    setBatteryState(BATTERY_LOW);  
  }
  else {
    setBatteryState(BATTERY_OK);
  }

#ifdef SERIAL_DEBUG
  if ((_frame - 1) % (BATT_VOLTAGE_FILT_DOWNSAMPLE << 4) == 0) {
    Serial.print("Battery voltage: ");
    Serial.print(_milliVolts);
    Serial.print("mV (");
    switch (_batteryState) {
      case BATTERY_OK: Serial.println("OK)"); break;
      case BATTERY_LOW: Serial.println("LOW)"); break;
      case BATTERY_CRITICAL: Serial.println("CRITICAL)"); break;
    }
  }
#endif
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
#ifdef DISABLE_SLEEP
  while (digitalRead(BUTTON_A_PIN) == LOW || digitalRead(BUTTON_B_PIN) == LOW) {
    FastLED.delay(5);
  }
  while (digitalRead(BUTTON_A_PIN) == HIGH && digitalRead(BUTTON_B_PIN) == HIGH) {
    FastLED.delay(5);
  }
#else
  Snooze.deepSleep(SnoozeConfig);
#endif
  Buttons.wakeup();
  if (ledsOn) {
    turnLedsOn();
  }
}

void PowerModule::setBrightness(uint8_t brightness) {
  _brightness = brightness;

  if (_dimMode) {
    brightness >>= LED_DIM_SHIFT;
    brightness = brightness < LED_DIM_MIN ? LED_DIM_MIN : brightness;
  }
  
  if (_lowPowerMode) {
    brightness = brightness < LOW_POWER_MAX_BRIGHTNESS ? brightness : LOW_POWER_MAX_BRIGHTNESS;
  }
  
  FastLED.setBrightness(brightness);
}

void PowerModule::setLowPowerMode(bool enable) {
  if (enable == _lowPowerMode) {
    return;
  }
  _lowPowerMode = enable;
#ifdef SERIAL_DEBUG
  Serial.print("Low-Power mode ");
  Serial.println(enable ? "enabled." : "disabled.");
#endif  
  setBrightness(_brightness);
}

void PowerModule::setDimMode(bool enable) {
  if (enable == _dimMode) {
    return;
  }
  _dimMode = enable;
#ifdef SERIAL_DEBUG
  Serial.print("Dim mode ");
  Serial.println(enable ? "enabled." : "disabled.");
#endif  
  setBrightness(_brightness);  
}

PowerModule Power;
