#pragma once

#include <Arduino.h>

const int TEMP_PIN = 9; 
const float VREF = 3.3;
const float ADC_MAX = 4095.0;

// TMP36: Vout(mV) = 500 + (tempC * 10)  →  tempC = (Vout_mV - 500) / 10
// Uses analogReadMilliVolts() for ESP32 built-in eFuse calibration

float readTemperatureC();
void  printTemperature();
