#pragma once

#include <Arduino.h>
#include <FastLED.h>

#define SERIAL_DEBUG

constexpr int LED_CLOCK_PIN = 7;
constexpr int LED_DATA_PIN = 8;
constexpr int LED_ENABLE_PIN = 9;
constexpr int LED_SENSE_PIN = 10;
constexpr int BUTTON_A_PIN = 4;
constexpr int BUTTON_B_PIN = 6;
constexpr int BATT_VOLTAGE_PIN = 21;

constexpr int LED_COUNT = 72;
constexpr int BRIGHTNESS = 255;

constexpr int FPS = 48;
constexpr unsigned long FRAME_TIME = 1000/FPS;

constexpr int EEPROM_ADDR_BRIGHTNESS = 0;
constexpr int EEPROM_ADDR_CUE_COUNT = 16;
constexpr int EEPROM_ADDR_FIRST_CUE = 18;
constexpr int EEPROM_CUE_SIZE = 2;
constexpr int MAX_CUES = 100;

constexpr int BATT_VOLTAGE_SCALER = 5981;

extern CRGB leds[LED_COUNT];
extern char transitionMask[LED_COUNT];

