#include "StateController.h"
#include "Config.h"
#include "DisplayController.h"
#include "DualButtonController.h"
#include <EEPROM.h>

SleepMode sleepMode;
PerformMode performMode;
BrightnessMode brightnessMode;
ProgramEnterMode programEnterMode;
ProgramMode programMode;

OperatingMode* StateController::_modes[MODE_COUNT] = {
  &sleepMode,
  &performMode,
  &brightnessMode,
  &programEnterMode,
  &programMode
};

void OperatingMode::execute(Command command) {
  switch (command) {
    case PERFORM_FIRST_CUE:
      Display.showFirstCue();
      break;
      
    case PERFORM_NEXT_CUE:
      Display.showNextCue();
      break;

    case PERFORM_PREVIOUS_CUE:
      Display.showPreviousCue();
      break;
      
    default:
      break;
  }
}

void SleepMode::enter(Command command) {
  execute(command);
  sleep();
}

void SleepMode::leave() {
  wakeUp();
}

void SleepMode::sleep() {
  Serial.println("Going to sleep...");
  Akira.turnLedsOff();
}

void SleepMode::wakeUp() {
  Serial.println("Waking up...");
  Akira.turnLedsOn();
}

constexpr uint8_t BrightnessMode::_levels[];
constexpr int BrightnessMode::DEFAULT_SETTING;

void BrightnessMode::initialize() {
  load();
  apply();
}

void BrightnessMode::execute(Command command) {
  switch (command) {
    case BRIGHTNESS_INCREASE:
      _currentSetting = _currentSetting < numLevels() - 1 ? _currentSetting + 1 : _currentSetting;
      break;
      
    case BRIGHTNESS_DECREASE:
      _currentSetting = _currentSetting > 0 ? _currentSetting - 1 : _currentSetting;
      break;
      
    default:
      OperatingMode::execute(command);
      return;
  }
  
  apply();
}

void BrightnessMode::leave() {
  save();
}

void BrightnessMode::reset() {
  _currentSetting = DEFAULT_SETTING;
  save();
  apply();
}

void BrightnessMode::save() {
  EEPROM.write(EEPROM_ADDR_BRIGHTNESS, _currentSetting);
}

void BrightnessMode::load() {
  _currentSetting = EEPROM.read(EEPROM_ADDR_BRIGHTNESS);
  if (_currentSetting >= numLevels()) {
    _currentSetting = numLevels() - 1;
  }
}

void BrightnessMode::apply() {
  Serial.print("Brightness: ");
  Serial.println(level());
  FastLED.setBrightness(level());
}

void StateController::initialize() {
  Buttons.update();
  if (Buttons.isButtonDown(BUTTON_A)) {
    factoryReset();
  }
  
  for (int i = 0; i < MODE_COUNT; ++i) {
    _modes[i]->initialize();
  }

  setOperatingMode(MODE_SLEEP);
}

bool StateController::areLedsOn() const {
  return digitalRead(LED_SENSE_PIN) == LOW;
}

void StateController::turnLedsOn() {
  Serial.println("Turning LEDs on.");
  digitalWrite(LED_ENABLE_PIN, HIGH);
  delay(1);
  if (!areLedsOn()) {
    Serial.println("Attempted to power up LEDs but no voltage detected.");
    return;
  }
  pinMode(LED_CLOCK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);  
}

void StateController::turnLedsOff() {
  Serial.println("Turning LEDs off.");
  pinMode(LED_CLOCK_PIN, INPUT);
  pinMode(LED_DATA_PIN, INPUT);
  digitalWrite(LED_ENABLE_PIN, LOW);
  delay(1);
  if (areLedsOn()) {
    Serial.println("Failed to power down LEDs.");
    FastLED.showColor(CRGB::Black);
    return;
  }
}

void StateController::factoryReset() {
  Serial.println("Factory reset!");
  turnLedsOn();
  brightnessMode.reset();
  FastLED.showColor(CRGB::White);
  FastLED.delay(400);
  FastLED.showColor(CRGB::Black);
  turnLedsOff();
}

void StateController::setOperatingModeWithCommand(Mode mode, Command command) {
  if (mode != _currentMode) {
    if (_currentMode >= 0) {
      _modes[_currentMode]->leave();
      Serial.print("Leaving ");
      Serial.print(_modes[_currentMode]->name());
      Serial.println(" mode.");
    }
    _currentMode = mode;
    Serial.print("Entering ");
    Serial.print(_modes[_currentMode]->name());
    Serial.println(" mode.");
    _modes[_currentMode]->enter(command);
  }
  else if (_currentMode >= 0 || command != DO_NOTHING)
  {
    _modes[_currentMode]->execute(command);
  }
}

StateController Akira;
