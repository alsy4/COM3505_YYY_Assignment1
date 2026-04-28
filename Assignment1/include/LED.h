#pragma once

#include <Arduino.h>

const int RED_LED_PIN    = 3;
const int YELLOW_LED_PIN = 4;
const int GREEN_LED_PIN  = 5;

// PWM constant to manually adjust the brightness later on
static const uint32_t LED_PWM_FREQ_HZ  = 5000;
static const uint8_t  LED_PWM_RES_BITS = 8;            // 0..255
static const uint8_t  LED_COUNT        = 3;

enum LedIndex { LED_RED = 0, LED_YELLOW = 1, LED_GREEN = 2 };

void ledInit();
void ledClear();
void ledSet(LedIndex i, uint8_t brightness);
void ledFlush();

void solidLED();
void blinkLED();
void chaseLED();
void flickerLED();
