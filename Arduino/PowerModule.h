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

private:
  void setBatteryState(BatteryState state);

  int _milliVolts = 0;
  BatteryState _batteryState = BATTERY_OK;
  int _lowThreshold = BATT_MILLIVOLT_LOW;
  int _criticalThreshold = BATT_MILLIVOLT_CRITICAL;
};

extern PowerModule Power;
