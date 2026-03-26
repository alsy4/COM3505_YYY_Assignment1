#include "LED.h"

void solidLED() {
    Serial.println("Setting LED solid");

    digitalWrite(RED_LED_PIN,    LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN,  HIGH);
}

void blinkLED() {
    Serial.println("Setting LED_PIN HIGH...");
    digitalWrite(RED_LED_PIN,    HIGH);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN,  HIGH);
    delay(1000);

    Serial.println("Setting LED_PIN LOW...");
    digitalWrite(RED_LED_PIN,    LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN,  LOW);
    delay(1000);
}

void chaseLED() {
    const int pins[]   = { RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN };
    const int numPins  = 3;
    const int chaseDelay = 200;

    Serial.println("Starting LED chase pattern...");

    // Chase forward
    for (int i = 0; i < numPins; i++) {
        digitalWrite(pins[i], HIGH);
        delay(chaseDelay);
        digitalWrite(pins[i], LOW);
    }

    // Chase backward
    for (int i = numPins - 1; i >= 0; i--) {
        digitalWrite(pins[i], HIGH);
        delay(chaseDelay);
        digitalWrite(pins[i], LOW);
    }
}

void flickerLED() {
    const int pins[]          = { RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN };
    const int numPins         = 3;
    const int flickerDuration = 2000;
    const int minDelay        = 30;
    const int maxDelay        = 150;

    Serial.println("Starting random LED flicker...");

    unsigned long startTime = millis();

    while (millis() - startTime < flickerDuration) {
        int randomPin = pins[random(0, numPins)];
        digitalWrite(randomPin, HIGH);
        delay(random(minDelay, maxDelay));
        digitalWrite(randomPin, LOW);
        delay(random(minDelay, maxDelay));
    }

    // Ensure all LEDs are off (or random state) after flickering
    for (int i = 0; i < numPins; i++) {
        digitalWrite(pins[i], random(0, 2));
    }

    Serial.println("Flicker complete.");
}