#pragma once

#include <Arduino.h>
#include <FastLED.h>

//#define INIT_EEPROM
#define SERIAL_DEBUG
//#define DISABLE_SLEEP
//#define DISABLE_BATTERY_LEVEL
//#define EXPERIMENTAL_ANIMATIONS
//#define SUPPORT_BROKEN_LIGHT_STAFF

#ifdef INIT_EEPROM
constexpr int BATT_MILLIVOLT_MAX = 5100;
#endif

constexpr int DEBUG1_PIN = 0;
constexpr int DEBUG2_PIN = 1;

constexpr int LED_CLOCK_PIN = 7;
constexpr int LED_DATA_PIN = 8;
constexpr int LED_ENABLE_PIN = 9;
#ifdef SUPPORT_BROKEN_LIGHT_STAFF
constexpr int LED_ENABLE_PIN2 = 11;
#endif
constexpr int LED_SENSE_PIN = 10;
constexpr int BUTTON_A_PIN = 4;
constexpr int BUTTON_B_PIN = 6;
constexpr int BATT_VOLTAGE_PIN = 21;

constexpr int BATT_MILLIVOLT_FULL = 4200;
constexpr int BATT_MILLIVOLT_LOW = 3100;
constexpr int BATT_MILLIVOLT_CRITICAL = 3000;
constexpr int BATT_MILLIVOLT_HYSTERESIS = 600;
constexpr int BATT_MILLIVOLT_OK1 = BATT_MILLIVOLT_LOW + (BATT_MILLIVOLT_HYSTERESIS >> 1);
constexpr int BATT_MILLIVOLT_OK2 = BATT_MILLIVOLT_LOW + BATT_MILLIVOLT_HYSTERESIS;
constexpr int BATT_VOLTAGE_FILT_DOWNSAMPLE = 10;
constexpr int BATT_VOLTAGE_FILT_EXP = 5;

constexpr int LED_COUNT = 76;
constexpr int LED_UI_SIDE_INDICATOR_SIZE = 4;
constexpr int LED_UI_MARGIN = 2;
constexpr int LED_UI_BATT_START = LED_UI_SIDE_INDICATOR_SIZE + LED_UI_MARGIN;
constexpr int LED_UI_BATT_SPAN = LED_COUNT - 2 * (LED_UI_SIDE_INDICATOR_SIZE + LED_UI_MARGIN);
constexpr int LED_UI_BATT_CRITICAL_SPAN = 4;
constexpr int LED_UI_BATT_CRITICAL_LOW = (BATT_MILLIVOLT_LOW - BATT_MILLIVOLT_CRITICAL) * (LED_UI_BATT_SPAN - LED_UI_BATT_CRITICAL_SPAN) / (BATT_MILLIVOLT_FULL - BATT_MILLIVOLT_CRITICAL);
constexpr int LED_UI_BATT_CRITICAL_OK1 = (BATT_MILLIVOLT_OK1 - BATT_MILLIVOLT_CRITICAL) * (LED_UI_BATT_SPAN - LED_UI_BATT_CRITICAL_SPAN) / (BATT_MILLIVOLT_FULL - BATT_MILLIVOLT_CRITICAL);
constexpr int LED_UI_BATT_CRITICAL_OK2 = (BATT_MILLIVOLT_OK2 - BATT_MILLIVOLT_CRITICAL) * (LED_UI_BATT_SPAN - LED_UI_BATT_CRITICAL_SPAN) / (BATT_MILLIVOLT_FULL - BATT_MILLIVOLT_CRITICAL);
constexpr int LED_UI_BATT_CRITICAL_MAX = LED_UI_BATT_SPAN - LED_UI_BATT_CRITICAL_SPAN;

#ifdef DISABLE_BATTERY_LEVEL
constexpr int LED_DIM_SHIFT = 0;
#else
constexpr int LED_DIM_SHIFT = 5;
#endif
constexpr uint8_t LED_DIM_MIN = 7;
constexpr uint8_t LOW_POWER_MAX_BRIGHTNESS = 63;

constexpr int FPS = 48;
constexpr unsigned long FRAME_TIME = 1000/FPS;

constexpr int EEPROM_ADDR_BATT_MILLIVOLT_MAX = 0;
constexpr int EEPROM_ADDR_BRIGHTNESS = 2;
constexpr int EEPROM_ADDR_CUE_COUNT = 16;
constexpr int EEPROM_ADDR_FIRST_CUE = 18;
constexpr int EEPROM_CUE_SIZE = 2;
constexpr int MAX_CUES = 100;

constexpr int ERROR_BATTERY_CRITICAL = 0;
constexpr int ERROR_LED_WONT_TURN_ON = 1;
constexpr int ERROR_LED_WONT_TURN_OFF = 2;

extern CRGB leds[LED_COUNT];
extern char transitionMask[LED_COUNT];

