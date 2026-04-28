#include "LED.h"

// ---------------- LED buffer ----------------
static const int     ledPins[LED_COUNT]     = { RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN };
static const uint8_t ledChannels[LED_COUNT] = { 0, 1, 2 };
static uint8_t       ledBuffer[LED_COUNT]   = { 0, 0, 0 };

void ledInit() {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        ledcSetup(ledChannels[i], LED_PWM_FREQ_HZ, LED_PWM_RES_BITS);
        ledcAttachPin(ledPins[i], ledChannels[i]);
    }
}

void ledClear() {
    for (uint8_t i = 0; i < LED_COUNT; i++) ledBuffer[i] = 0;
}

void ledSet(LedIndex i, uint8_t brightness) {
    ledBuffer[i] = brightness;
}

void ledFlush() {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        ledcWrite(ledChannels[i], ledBuffer[i]);
    }
}

// ---------------- Patterns ----------------
void solidLED() {
    Serial.println("Setting LED solid");

    ledClear();
    ledSet(LED_GREEN, 255);
    ledFlush();
}

void blinkLED() {
    Serial.println("Setting LED_PIN HIGH...");
    ledSet(LED_RED,    255);
    ledSet(LED_YELLOW, 255);
    ledSet(LED_GREEN,  255);
    ledFlush();
    delay(1000);

    Serial.println("Setting LED_PIN LOW...");
    ledClear();
    ledFlush();
    delay(1000);
}

void chaseLED() {
    const int chaseDelay = 200;

    Serial.println("Starting LED chase pattern...");

    // Chase forward
    for (int i = 0; i < LED_COUNT; i++) {
        ledClear();
        ledSet((LedIndex)i, 255);
        ledFlush();
        delay(chaseDelay);
    }

    // Chase backward
    for (int i = LED_COUNT - 1; i >= 0; i--) {
        ledClear();
        ledSet((LedIndex)i, 255);
        ledFlush();
        delay(chaseDelay);
    }

    ledClear();
    ledFlush();
}

void flickerLED() {
    const int flickerDuration = 2000;
    const int minDelay        = 30;
    const int maxDelay        = 150;

    Serial.println("Starting random LED flicker...");

    unsigned long startTime = millis();

    while (millis() - startTime < flickerDuration) {
        LedIndex which = (LedIndex)random(0, LED_COUNT);

        ledSet(which, 255);
        ledFlush();
        delay(random(minDelay, maxDelay));

        ledSet(which, 0);
        ledFlush();
        delay(random(minDelay, maxDelay));
    }

    ledClear();
    ledFlush();

    Serial.println("Flicker complete.");
}
