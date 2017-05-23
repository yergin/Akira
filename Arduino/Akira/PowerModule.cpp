#include "PowerModule.h"
#include "DualButtonController.h"
#include "ErrorStatus.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Snooze.h>

SnoozeDigital Digital;
SnoozeBlock SnoozeConfig(Digital);

union UnsignedShort {
  uint16_t val;
  char data[2];
};

void PowerModule::initialize() {
  Digital.pinMode(BUTTON_A_PIN, INPUT_PULLUP, FALLING);
  Digital.pinMode(BUTTON_B_PIN, INPUT_PULLUP, FALLING);
  analogReference(INTERNAL);

  UnsignedShort maxMillivolts;
#ifdef INIT_EEPROM
  maxMillivolts.val = BATT_MILLIVOLT_MAX;
  EEPROM.write(EEPROM_ADDR_BATT_MILLIVOLT_MAX, maxMillivolts.data[0]);
  EEPROM.write(EEPROM_ADDR_BATT_MILLIVOLT_MAX + 1, maxMillivolts.data[1]);
#ifdef SERIAL_DEBUG
  Serial.print("Storing max battery voltage: ");
  Serial.print(maxMillivolts.val);
  Serial.println("mV");
#endif
#else
  maxMillivolts.data[0] = EEPROM.read(EEPROM_ADDR_BATT_MILLIVOLT_MAX);
  maxMillivolts.data[1] = EEPROM.read(EEPROM_ADDR_BATT_MILLIVOLT_MAX + 1);
#ifdef SERIAL_DEBUG
  Serial.print("Retrieving max battery voltage: ");
  Serial.print(maxMillivolts.val);
  Serial.println("mV");
#endif
#endif
  _battMillivoltMax = maxMillivolts.val;
}

void PowerModule::resetVoltage() {
  _frame = 0;
  _milliVolts = 0;
}

void PowerModule::update() {
  if (_frame++ % BATT_VOLTAGE_FILT_DOWNSAMPLE) {
    return;
  }

#ifdef DISABLE_BATTERY_LEVEL
  setBatteryState(BATTERY_OK);
#else
  int milliVolts = (analogRead(BATT_VOLTAGE_PIN) * _battMillivoltMax) >> 10;
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
#endif

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
#ifdef SUPPORT_BROKEN_LIGHT_STAFF
  digitalWrite(LED_ENABLE_PIN2, HIGH);
#endif
  delay(1);
  if (!areLedsOn()) {
#ifdef SERIAL_DEBUG
    Serial.println("Attempted to power up LEDs but no voltage detected.");
#endif
    Errors.setError(ERROR_LED_WONT_TURN_ON);
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
#ifdef SUPPORT_BROKEN_LIGHT_STAFF
  digitalWrite(LED_ENABLE_PIN2, LOW);
#endif
  delay(1);
  if (areLedsOn()) {
#ifdef SERIAL_DEBUG
    Serial.println("Failed to power down LEDs.");
#endif
    Errors.setError(ERROR_LED_WONT_TURN_OFF);
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
  
  Errors.resetDisplay();
  digitalWrite(DEBUG1_PIN, HIGH);
  
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

  digitalWrite(DEBUG1_PIN, LOW);
  resetVoltage();

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
