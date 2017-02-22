#include "StateController.h"
#include "Config.h"
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
      Akira.showFirstCue();
      break;
      
    case PERFORM_NEXT_CUE:
      Akira.showNextCue();
      break;

    case PERFORM_PREVIOUS_CUE:
      Akira.showPreviousCue();
      break;
      
    default:
      break;
  }
}

void OperatingMode::shortBlink() {
  FastLED.showColor(CRGB::Black);
  delay(80);
}

void OperatingMode::longBlink() {
  FastLED.showColor(CRGB::Black);
  delay(120);
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

void BrightnessMode::enter(Command command) {
  execute(command);
  shortBlink();
}

void BrightnessMode::leave() {
  save();
  longBlink();
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

void ProgramEnterMode::enter(Command command) {
  shortBlink();
  execute(command);
}

void ProgramEnterMode::execute(Command command) {
  switch (command) {
    case PROGRAM_FIRST_CUE:
      Akira.showFirstCue();
      break;

    case PROGRAM_ERASE_ALL_CUES:
      Akira.eraseAllCues();
      break;

    default:
      OperatingMode::execute(command);
  }
}

void ProgramMode::enter(Command command) {
  Akira.enterProgramMode();
  shortBlink();
  execute(command);
}

void ProgramMode::execute(Command command) {
  switch (command) {
    case PROGRAM_FIRST_CUE:
      Akira.showFirstCue();
      break;
      
    case PROGRAM_NEXT_CUE:
      Akira.showNextCue();
      break;
      
    case PROGRAM_DELETE_CUE:
      Akira.deleteCurrentCue();
      break;
      
    case PROGRAM_INSERT_CUE:
      Akira.insertCue();
      break;
    
    case PROGRAM_ERASE_ALL_CUES:
      Akira.eraseAllCues();
      break;
    
    case PROGRAM_CHANGE_COLOR1:
      Akira.changeColor1();
      break;
      
    case PROGRAM_CHANGE_COLOR2:
      Akira.changeColor2();
      break;
      
    case PROGRAM_CHANGE_ANIMATION:
      Akira.changeAnimation();
      break;

    default:
      OperatingMode::execute(command);
  }
}

void ProgramMode::leave() {
  Akira.exitProgramMode();
  longBlink();
}

void StateController::factoryReset() {
  Serial.println("Factory reset!");
  turnLedsOn();
  brightnessMode.reset();
  FastLED.showColor(CRGB::White);

  storeDemoCues();
  
  FastLED.delay(400);
  FastLED.showColor(CRGB::Black);
  turnLedsOff();
}

void StateController::storeDemoCues() {
  EEPROM.write(EEPROM_ADDR_CUE_COUNT, AkiraAnimation::demoCueCount());
  int addr = EEPROM_ADDR_FIRST_CUE;
  for (int i = 0; i < AkiraAnimation::demoCueCount(); ++i) {
    AkiraAnimation* animation = AkiraAnimation::loadDemoCue(i);
    animation->saveToEeprom(&addr);
    delete animation;
  }
}

void StateController::initialize() {
  Buttons.update();
  if (Buttons.isButtonDown(BUTTON_A)) {
    factoryReset();
  }

  loadCuesFromEeprom();
  
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

void StateController::showFirstCue() {
  _currentCue = 0;
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::showNextCue() {
  _currentCue = _currentCue < _cueCount - 1 ? _currentCue + 1 : 0;
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::showPreviousCue() {
  _currentCue = _currentCue > 0 ? _currentCue - 1 : _cueCount - 1;
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::enterProgramMode() {
  // Nothing to do
}

void StateController::deleteCurrentCue() {
  Serial.print("Deleting cue #");
  Serial.print(_currentCue + 1);
  Serial.println(".");
  
  for (int i = _currentCue; i < _cueCount - 1; ++i) {
    _cues[i] = _cues[i+1];
  }
  
  if (_cueCount > 1) {
    _cueCount = _cueCount - 1;
  }
  else {
    _cues[0].color1 = PRESET_COL_WHITE;
    _cues[0].color2 = PRESET_COL_BLACK;
    _cues[0].animation = PRESET_ANIM_GRADIENT;
  }
  
  if (_currentCue >= _cueCount) {
    _currentCue = _cueCount - 1;
  }
  
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::insertCue() {
  if (_cueCount >= MAX_CUES) {
    Serial.print("Maximum cue limit reached.");
    return;
  }
  
  Serial.print("Inserting cue at position #");
  Serial.print(_currentCue + 1);
  Serial.println(".");

  for (int i = _cueCount; i > _currentCue; --i) {
    _cues[i] = _cues[i-1];
  }

  _cueCount = _cueCount + 1;
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::eraseAllCues() {
  Serial.print("Erasing all cues.");

  _currentCue = 0;
  _cueCount = 1;
  _cues[0].color1 = PRESET_COL_WHITE;
  _cues[0].color2 = PRESET_COL_BLACK;
  _cues[0].animation = PRESET_ANIM_GRADIENT;
  
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::changeColor1() {
  Serial.println("Changing color #1.");
  _cues[_currentCue].color1 = static_cast<ColorPreset>((_cues[_currentCue].color1 + 1) % PRESET_COL_COUNT);
  
  if (_targetAnimation) {
    _targetAnimation->setColorPreset1(_cues[_currentCue].color1);
  }
}

void StateController::changeColor2() {
  Serial.println("Changing color #2.");
  _cues[_currentCue].color2 = static_cast<ColorPreset>((_cues[_currentCue].color2 + 1) % PRESET_COL_RAINBOW);
  if (_cues[_currentCue].color2 == _cues[_currentCue].color1) {
    _cues[_currentCue].color2 = static_cast<ColorPreset>((_cues[_currentCue].color2 + 1) % PRESET_COL_RAINBOW);
  }
  
  if (_targetAnimation) {
    _targetAnimation->setColorPreset2(_cues[_currentCue].color2);
  }
}

void StateController::changeAnimation() {
  Serial.println("Changing animation.");
  _cues[_currentCue].animation = static_cast<AnimationPreset>((_cues[_currentCue].animation + 1) % PRESET_ANIM_COUNT);
  setNextAnimation(createAnimation(_currentCue));  
}

void StateController::exitProgramMode() {
  storeCuesInEeprom();
}

void StateController::loadCuesFromEeprom() {
  Serial.println("Loading cues.");
  _cueCount = EEPROM.read(EEPROM_ADDR_CUE_COUNT);
  int addr = EEPROM_ADDR_FIRST_CUE;
  for (int i = 0; i < _cueCount; ++i) {
    AkiraAnimation* animation = AkiraAnimation::loadFromEeprom(&addr);
    animation->copyToDescription(&_cues[i]);
    delete animation;
  }      
}

void StateController::storeCuesInEeprom() {
  Serial.println("Storing cues.");
  EEPROM.write(EEPROM_ADDR_CUE_COUNT, _cueCount);
  int addr = EEPROM_ADDR_FIRST_CUE;
  for (int i = 0; i < _cueCount; ++i) {
    AkiraAnimation* animation = AkiraAnimation::create(_cues[i]);
    animation->saveToEeprom(&addr);
    delete animation;
  }
}

AkiraAnimation* StateController::createAnimation(int index) {
  return AkiraAnimation::create(_cues[index]);
}

void StateController::setNextAnimation(AkiraAnimation* animation) {
  Serial.print("Displaying cue #");
  Serial.println(_currentCue + 1);
  
  if (_sourceAnimation) {
    delete _sourceAnimation;
  }
  
  _sourceAnimation = _targetAnimation;
  _targetAnimation = animation;
  
  if (!_targetAnimation) {
    return;
  }
  
  if (transition()) {
    transition()->reset();
    _sourceAnimation->setMask(transition()->mask(), Animation::PRE_BLEND);
    _targetAnimation->setMask(transition()->mask(), Animation::OVERLAY);
  }
  else {
    _targetAnimation->setMask(0, Animation::OVERLAY);
  }
}

void StateController::update() {
  if (!_targetAnimation) {
    return;
  }
  
  if (transition() && !transition()->isCompleted()) {
    _targetAnimation->transition()->advance();
    _sourceAnimation->draw();
    _targetAnimation->draw();

    if (transition()->isCompleted()) {
      _sourceAnimation->setMask(0, Animation::PRE_BLEND);
      _targetAnimation->setMask(0, Animation::OVERLAY);
    }
  }
  else {
    _targetAnimation->draw();
  }
}

Transition* StateController::transition() const {
  return _targetAnimation && _sourceAnimation ? _targetAnimation->transition() : 0;
}

StateController Akira;
