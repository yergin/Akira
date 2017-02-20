#include "StateController.h"
#include "Config.h"
#include "CueDisplay.h"
#include <EEPROM.h>

void OperatingMode::execute(Command command) {
  switch (command) {
    case PERFORM_FIRST_CUE:
      CueDisplay.showFirst();
      break;
      
    case PERFORM_NEXT_CUE:
      CueDisplay.showNext();
      break;

    case PERFORM_PREVIOUS_CUE:
      CueDisplay.showPrevious();
      break;
      
    default:
      break;
  }
}

void OffMode::enter(Command command) {
  execute(command);
  sleep();
}

void OffMode::leave() {
  wakeUp();
}

void OffMode::sleep() {
  Serial.println("Going to sleep...");
  pinMode(LED_CLOCK_PIN, INPUT);
  pinMode(LED_DATA_PIN, INPUT);
  digitalWrite(LED_ENABLE_PIN, LOW);
  delay(1);
  if (!areLedsOn()) {
    Serial.print("Failed to power down LEDs.\n");
    return;
  }
}

void OffMode::wakeUp() {
  Serial.println("Waking up...");
  digitalWrite(LED_ENABLE_PIN, HIGH);
  delay(1);
  if (!areLedsOn()) {
    Serial.print("Attempted to power up LEDs but no voltage detected.\n");
    return;
  }
  pinMode(LED_CLOCK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);
}

bool OffMode::areLedsOn() const {
  return digitalRead(LED_SENSE_PIN) == LOW;
}

constexpr uint8_t BrightnessMode::_levels[];

void BrightnessMode::initialize() {
  _currentLevel = EEPROM.read(0);
  if (_currentLevel >= numLevels()) {
    _currentLevel = numLevels() - 1;
  }
  FastLED.setBrightness(level());
}

void BrightnessMode::execute(Command command) {
  switch (command) {
    case BRIGHTNESS_INCREASE:
      _currentLevel = _currentLevel < numLevels() - 1 ? _currentLevel + 1 : _currentLevel;
      break;
      
    case BRIGHTNESS_DECREASE:
      _currentLevel = _currentLevel > 0 ? _currentLevel - 1 : _currentLevel;
      break;
      
    default:
      OperatingMode::execute(command);
      return;
  }

  Serial.print("Brightness = ");
  Serial.println(level());
  FastLED.setBrightness(level());
}

void BrightnessMode::leave() {
  EEPROM.write(0, _currentLevel);
}

void StateController::initializeModes() {
  for (int i = 0; i < MODE_COUNT; ++i) {
    _modes[i]->initialize();
  }
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

OffMode offMode;
PerformMode performMode;
BrightnessMode brightnessMode;
ProgramEnterMode programEnterMode;
ProgramMode programMode;

OperatingMode* StateController::_modes[MODE_COUNT] = {
  &offMode,
  &performMode,
  &brightnessMode,
  &programEnterMode,
  &programMode
};

StateController Akira;
