#pragma once

#include <Arduino.h>
#include <FastLED.h>

//#define SERIAL_DEBUG
//#define EXPERIMENTAL_ANIMATIONS

constexpr int LED_CLOCK_PIN = 7;
constexpr int LED_DATA_PIN = 8;
constexpr int LED_ENABLE_PIN = 9;
constexpr int LED_SENSE_PIN = 10;
constexpr int BUTTON_A_PIN = 4;
constexpr int BUTTON_B_PIN = 6;
constexpr int BATT_VOLTAGE_PIN = 21;

constexpr int BATT_MILLIVOLT_MAX = 5580;//5981;
constexpr int BATT_MILLIVOLT_FULL = 4200;
constexpr int BATT_MILLIVOLT_LOW = 3500;
constexpr int BATT_MILLIVOLT_CRITICAL = 3300;
constexpr int BATT_MILLIVOLT_HYSTERESIS = 200;
constexpr int BATT_VOLTAGE_FILT_EXP = 3;

constexpr int LED_COUNT = 72;
constexpr int LED_UI_SIDE_INDICATOR_SIZE = 4;
constexpr int LED_UI_MARGIN = 2;
constexpr int LED_UI_BATT_START = LED_UI_SIDE_INDICATOR_SIZE + LED_UI_MARGIN;
constexpr int LED_UI_BATT_SPAN = LED_COUNT - 2 * (LED_UI_SIDE_INDICATOR_SIZE + LED_UI_MARGIN);
constexpr int LED_UI_BATT_CRITICAL_SPAN = 4;
constexpr int LED_UI_BATT_LOW_SPAN = (BATT_MILLIVOLT_LOW - BATT_MILLIVOLT_CRITICAL) * (LED_UI_BATT_SPAN - LED_UI_BATT_CRITICAL_SPAN) / (BATT_MILLIVOLT_FULL - BATT_MILLIVOLT_CRITICAL);
constexpr int LED_UI_BATT_OK_SPAN = LED_UI_BATT_SPAN - LED_UI_BATT_CRITICAL_SPAN - LED_UI_BATT_LOW_SPAN;

constexpr int FPS = 48;
constexpr unsigned long FRAME_TIME = 1000/FPS;

constexpr int EEPROM_ADDR_BRIGHTNESS = 0;
constexpr int EEPROM_ADDR_CUE_COUNT = 16;
constexpr int EEPROM_ADDR_FIRST_CUE = 18;
constexpr int EEPROM_CUE_SIZE = 2;
constexpr int MAX_CUES = 100;

extern CRGB leds[LED_COUNT];
extern char transitionMask[LED_COUNT];

