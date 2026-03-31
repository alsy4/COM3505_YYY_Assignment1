#include <Arduino.h>
#include <WebServer.h>
#include <Main.h>

#include "LED.h"
#include "Wifi_Connect.h"
#include "Temperature.h"

// Local HTTP server running on port 80
WebServer webServer(80);
const int BUTTON_PIN = 6;

enum LedState {
    LED_BLINK,
    LED_FLICKER,
    LED_SOLID,
    LED_CHASE
};

volatile LedState currentLedState = LED_BLINK;
volatile bool buttonPressed = false;

// ------------------------------------------------------------
// ISR: flag the button press immediately (runs during any delay)
// ------------------------------------------------------------
void IRAM_ATTR buttonISR() {
    buttonPressed = true;
}

// ------------------------------------------------------------
// Process the button flag — only acts during LED_FLICKER
// ------------------------------------------------------------
void handleButton() {
    if (buttonPressed) {
        buttonPressed = false;
        if (currentLedState == LED_FLICKER || currentLedState == LED_CHASE) {
            Serial.println("Button pressed — switching to blink.");
            currentLedState = LED_BLINK;
        }
    }
}
 

void setup() {
    pinMode(RED_LED_PIN,    OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN,  OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // pinMode(TEMP_PIN, INPUT);
    
    analogReadResolution(12);

    // Increase ADC input range for better measurement
    analogSetPinAttenuation(TEMP_PIN, ADC_11db);
    Serial.begin(115200);
    delay(3000);

    // Connect to Wi-Fi network
    // connectWiFi();

    // Connect to server
    // if (!connectServer()) {
    //     Serial.println("Could not connect to server, restarting...");
    //     delay(10000);
    //     ESP.restart();
    // }

    // Send the GET request
    // sendGetRequest();

    // --------------------------------------------------------
    // Start local web server
    // --------------------------------------------------------
    // webServer.on("/", []() {
    //     webServer.send(200,
    //                    "text/html",
    //                    "<h1>ESP32 Ex08 Home Test</h1><p>Local page works!</p>");
    // });
    // webServer.begin();

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

    Serial.println("Local web server started");
}

void loop() {
    // Button test
    if (buttonPressed) {
        buttonPressed = false;
        Serial.println("Button pressed!");
    }

    // Handle incoming browser requests
    webServer.handleClient();

    // Serial.print("Wifi IP Address: ");
    // Serial.println(WiFi.localIP());

    static unsigned long lastTempMs = 0;
    if (millis() - lastTempMs >= 2000) {
        printTemperature();
        lastTempMs = millis();
    }


    // Temperature exceeds threshold: switch to chase immediately, then flicker after 5s
    static unsigned long tempExceedTime = 0;
    if (currentLedState == LED_BLINK || currentLedState == LED_CHASE) {
        float tempC = readTemperatureC();
        if (tempC > TEMP_THRESHOLD) {
            if (tempExceedTime == 0) {
                tempExceedTime = millis();
                currentLedState = LED_CHASE;
                Serial.println("Temperature exceeded threshold — switching to chase, 5s timer started.");
            } else if (currentLedState == LED_CHASE && millis() - tempExceedTime >= 5000) {
                Serial.println("5s elapsed — switching to flicker.");
                currentLedState = LED_FLICKER;
                tempExceedTime = 0;
            }
        } else {
            if (tempExceedTime != 0) {
                Serial.println("Temperature dropped below threshold — timer reset, back to blink.");
                currentLedState = LED_BLINK;
                tempExceedTime = 0;
            }
        }
    }

    switch (currentLedState) {
        case LED_BLINK:
            blinkLED();
            break;
        case LED_FLICKER:
            flickerLED();
            handleButton();
            break;
        case LED_SOLID:
            solidLED();
            break;
        case LED_CHASE:
            chaseLED();
            handleButton();
            break;
    }
    

}