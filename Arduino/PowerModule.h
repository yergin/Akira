#pragma once

#include "Config.h"

enum BatteryState {
  BATTERY_OK = 0,
  BATTERY_LOW,
  BATTERY_CRITICAL
};
  
class PowerModule {
public:
  void initialize();
  void update();
  int milliVolts() const { return _milliVolts; }
  BatteryState batteryState() const { return _batteryState; }
  bool areLedsOn() const;
  void turnLedsOn();
  void turnLedsOff();
  void deepSleep();
  void setBrightness(uint8_t brightness);
  void setLowPowerMode(bool enable);
  bool lowPowerMode() const { return _lowPowerMode; }
  void setDimMode(bool enable);
  bool dimMode() const { return _dimMode; }

private:
  void setBatteryState(BatteryState state);

  int _milliVolts = 0;
  BatteryState _batteryState = BATTERY_OK;
  int _lowThreshold = BATT_MILLIVOLT_LOW;
  int _criticalThreshold = BATT_MILLIVOLT_CRITICAL;
  unsigned int _frame = 0;
  uint8_t _brightness = 255;
  bool _lowPowerMode = false;
  bool _dimMode = false;
};

extern PowerModule Power;
