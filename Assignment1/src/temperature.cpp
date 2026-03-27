#include <Arduino.h>
#include "Temperature.h"

float readTemperatureC() {
    int   adcValue     = analogRead(TEMP_PIN);
    float voltage = adcValue * VREF / ADC_MAX; // millivolts
    return (voltage-0.5) * 100;
}

void printTemperature() {
    float tempC = readTemperatureC();
    Serial.print("Temperature: ");
    Serial.print(tempC, 1);
    Serial.println(" °C");
}
