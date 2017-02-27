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
  Power.turnLedsOff();
  switch (command) {
    case SLEEP_IMMEDIATE:
      sleep();
      break;

    default:
      execute(command);
  }
}

void SleepMode::leave() {
  Power.turnLedsOn();  
}

void SleepMode::sleep() {
  Power.deepSleep();
}

void SleepMode::update() {
  if (Buttons.triggered(BUTTONS_A_B_TOGETHER, RELEASE)) {
    sleep();
  }
}

void ProgramEnterMode::enter(Command command) {
  shortBlink();
  execute(command);
  Akira.showBattery();
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

void ProgramEnterMode::leave() {
  Akira.hideBattery();
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

    case BRIGHTNESS_MAXIMUM:
      _currentSetting = numLevels() - 1;
      break;
      
    case BRIGHTNESS_MINIMUM:
      _currentSetting = 0;
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
#ifdef SERIAL_DEBUG
  Serial.print("Brightness: ");
  Serial.println(level());
#endif
  Power.setBrightness(level());
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
#ifdef SERIAL_DEBUG
  Serial.println("Factory reset!");
#endif
  Power.turnLedsOn();
  brightnessMode.reset();
  FastLED.showColor(CRGB::White);

  storeDemoCues();
  
  FastLED.delay(400);
  FastLED.showColor(CRGB::Black);
  Power.turnLedsOff();
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
  Power.initialize();
  Buttons.update();
  if (Buttons.isButtonDown(BUTTON_A)) {
    factoryReset();
  }

  loadCuesFromEeprom();
  
  for (int i = 0; i < MODE_COUNT; ++i) {
    _modes[i]->initialize();
  }

  delay(200);
  
  setOperatingModeWithCommand(MODE_SLEEP, SLEEP_IMMEDIATE);
}

void StateController::update() {
  Power.update();
  if (Power.batteryState() == BATTERY_CRITICAL) {
    setOperatingMode(MODE_SLEEP);
  }
  else if (Power.batteryState() == BATTERY_LOW) {
    Power.setLowPowerMode(true);
  }
  else {
    Power.setLowPowerMode(false);    
  }
  respondToButtons();
  _modes[_currentMode]->update();
  updateAnimations();
  FastLED.show();
}  

void StateController::respondToButtons() {
  using namespace DualButtons;
  
  Buttons.update();
  if (!Buttons.triggered()) {
    return;
  }

  for (int i = 0; i < StateController::transitionTableEntryCount(); ++i) {
    const StateTransition* transition = &stateTransitionTable[i];
    if (transition->currentMode != currentMode() || !Buttons.triggered(transition->button, transition->buttonEvent)) {
      continue;
    }

    setOperatingModeWithCommand(transition->nextMode, transition->command);
    Buttons.performRequest(transition->buttonRequest);
    break;
  }
}

void StateController::setOperatingModeWithCommand(Mode mode, Command command) {
  if (mode != _currentMode) {
    if (_currentMode >= 0) {
      _modes[_currentMode]->leave();
#ifdef SERIAL_DEBUG
      Serial.print("Leaving ");
      Serial.print(_modes[_currentMode]->name());
      Serial.println(" mode.");
#endif
    }
    _currentMode = mode;
#ifdef SERIAL_DEBUG
    Serial.print("Entering ");
    Serial.print(_modes[_currentMode]->name());
    Serial.println(" mode.");
#endif
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
#ifdef SERIAL_DEBUG
  Serial.print("Deleting cue #");
  Serial.print(_currentCue + 1);
  Serial.println(".");
#endif
  for (int i = _currentCue; i < _cueCount - 1; ++i) {
    _cues[i] = _cues[i+1];
  }
  
  if (_cueCount > 1) {
    _cueCount = _cueCount - 1;
  }
  else {
    _cues[0].color1 = PRESET_COL_WHITE;
    _cues[0].color2 = PRESET_COL_RAINBOW_OR_BLACK;
    _cues[0].animation = PRESET_ANIM_GRADIENT;
  }
  
  if (_currentCue >= _cueCount) {
    _currentCue = _cueCount - 1;
  }
  
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::insertCue() {
  if (_cueCount >= MAX_CUES) {
#ifdef SERIAL_DEBUG
    Serial.print("Maximum cue limit reached.");
#endif
    return;
  }
  
#ifdef SERIAL_DEBUG
  Serial.print("Inserting cue at position #");
  Serial.print(_currentCue + 1);
  Serial.println(".");
#endif

  for (int i = _cueCount; i > _currentCue; --i) {
    _cues[i] = _cues[i-1];
  }

  _cueCount = _cueCount + 1;
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::eraseAllCues() {
#ifdef SERIAL_DEBUG
  Serial.print("Erasing all cues.");
#endif
  _currentCue = 0;
  _cueCount = 1;
  _cues[0].color1 = PRESET_COL_WHITE;
  _cues[0].color2 = PRESET_COL_RAINBOW_OR_BLACK;
  _cues[0].animation = PRESET_ANIM_GRADIENT;
  
  setNextAnimation(createAnimation(_currentCue));
}

void StateController::changeColor1() {
#ifdef SERIAL_DEBUG
  Serial.println("Changing color #1.");
#endif
  _cues[_currentCue].color1 = static_cast<ColorPreset>((_cues[_currentCue].color1 + 1) % PRESET_COL_COUNT);
  
  if (_targetAnimation) {
    _targetAnimation->setColorPreset1(_cues[_currentCue].color1);
  }
}

void StateController::changeColor2() {
  if (_cues[_currentCue].color1 == PRESET_COL_RAINBOW_OR_BLACK) {
    return;
  }
  
#ifdef SERIAL_DEBUG
  Serial.println("Changing color #2.");
#endif
  _cues[_currentCue].color2 = static_cast<ColorPreset>((_cues[_currentCue].color2 + 1) % PRESET_COL_COUNT);
  if (_cues[_currentCue].color2 == _cues[_currentCue].color1) {
    _cues[_currentCue].color2 = static_cast<ColorPreset>((_cues[_currentCue].color2 + 1) % PRESET_COL_COUNT);
  }
  
  if (_targetAnimation) {
    _targetAnimation->setColorPreset2(_cues[_currentCue].color2);
  }
}

void StateController::changeAnimation() {
#ifdef SERIAL_DEBUG
  Serial.println("Changing animation.");
#endif
  _cues[_currentCue].animation = static_cast<AnimationPreset>((_cues[_currentCue].animation + 1) % PRESET_ANIM_COUNT);
  setNextAnimation(createAnimation(_currentCue));  
}

void StateController::exitProgramMode() {
  storeCuesInEeprom();
}

void StateController::loadCuesFromEeprom() {
#ifdef SERIAL_DEBUG
  Serial.println("Loading cues.");
#endif
  _cueCount = EEPROM.read(EEPROM_ADDR_CUE_COUNT);
  int addr = EEPROM_ADDR_FIRST_CUE;
  for (int i = 0; i < _cueCount; ++i) {
    AkiraAnimation* animation = AkiraAnimation::loadFromEeprom(&addr);
    animation->copyToDescription(&_cues[i]);
    delete animation;
  }      
}

void StateController::storeCuesInEeprom() {
#ifdef SERIAL_DEBUG
  Serial.println("Storing cues.");
#endif
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
#ifdef SERIAL_DEBUG
  Serial.print("Displaying cue #");
  Serial.println(_currentCue + 1);
#endif
  if (_sourceAnimation) {
    delete _sourceAnimation;
  }
  
  _sourceAnimation = _targetAnimation;
  _targetAnimation = animation;
  
  if (!_targetAnimation) {
    return;
  }
  
  if (allowFollowingTransition() && transition()) {
    transition()->reset();
    _sourceAnimation->setMask(transition()->mask(), Animation::PRE_BLEND);
    _targetAnimation->setMask(transition()->mask(), Animation::OVERLAY);
  }
  else {
    _targetAnimation->setMask(0, Animation::OVERLAY);
  }
}

void StateController::updateAnimations() {
  if (!_targetAnimation) {
    return;
  }
  
  if (allowFollowingTransition() && transition() && !transition()->isCompleted()) {
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

StateController Akira;
