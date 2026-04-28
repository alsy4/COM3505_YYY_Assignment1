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

unsigned long lastTempRead = 0;
unsigned long lastDataSend = 0;
unsigned long tempExceedTime = 0;

void sendDataToFlaskServer();

// ------------------------------------------------------------
// ISR: flag the button press immediately
// ------------------------------------------------------------
void IRAM_ATTR buttonISR() {
    buttonPressed = true;
}

// ------------------------------------------------------------
// Process the button flag - full reset to NORMAL (SOLID green)
// regardless of current state, and push the new state to Flask
// immediately so the dashboard reflects the reset without
// waiting for the next periodic send.
// ------------------------------------------------------------
void handleButton() {
    if (!buttonPressed) return;
    buttonPressed = false;

    Serial.println("Button pressed — resetting to NORMAL (SOLID).");
    currentLedState = LED_SOLID;
    tempExceedTime = 0;

    ledClear();
    ledFlush();

    sendDataToFlaskServer();
    lastDataSend = millis();
}

// ----------------------------------------
// send data to python plask Server
// -------------------------------------
void sendDataToFlaskServer() {
    // LED state for flask server
    String stateString;
    switch(currentLedState) {
        case LED_SOLID:   stateString = "LED_SOLID"; break;
        case LED_BLINK:   stateString = "LED_BLINK"; break;
        case LED_CHASE:   stateString = "LED_CHASE"; break;
        case LED_FLICKER: stateString = "LED_FLICKER"; break;
        default:          stateString = "UNKNOWN"; break;
    }
    
    // current temperature
    float tempC = readTemperatureC();
    
    String url = "/sensor?temperature=";
    url += String(tempC);
    url += "&state=";
    url += stateString;
    url += "&email=";
    url += myEmail;
    
    Serial.print("Sending to Flask server: ");
    Serial.print(url);
    
    // connct
    if (client.connect(serverHost, serverPort)) {
        // HTTP GET request
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + serverHost + "\r\n" +
                     "Connection: close\r\n\r\n");
        delay(100);
        
        while (client.available()) {
            String line = client.readStringUntil('\n');
            Serial.println("Response: " + line);
        }
        
        client.stop();
        Serial.println(" Data sent successful");
    } else {
        Serial.println("Failed to connect");
    }
}

// -----------------------------------------------
// update LED pattern
// --------------------------------------------------------
void updatePatternByTemperature() {
    float tempC = readTemperatureC();
    
    switch(currentLedState) {
        case LED_SOLID:
            if (tempC > TEMP_THRESHOLD) {
                Serial.println("Temperature threshold reach! Switch to BLINK");
                currentLedState = LED_BLINK;
            }
            break;
            
        case LED_BLINK:
            if (tempC > TEMP_THRESHOLD) {
                if (tempExceedTime == 0) {
                    tempExceedTime = millis();
                    Serial.println("Timer start: 5 seconds until CHASE mode");
                } else if (millis() - tempExceedTime >= 5000) {
                    Serial.println("5 seconds gone! Switch to CHASE");
                    currentLedState = LED_CHASE;
                    tempExceedTime = 0;
                }
            } else {
                if (tempExceedTime != 0) {
                    Serial.println("Temperature normal, returning to SOLID");
                    currentLedState = LED_SOLID;
                    tempExceedTime = 0;
                }
            }
            break;
            
        case LED_CHASE:
            if (tempC > TEMP_THRESHOLD) {
                if (tempExceedTime == 0) {
                    tempExceedTime = millis();
                } else if (millis() - tempExceedTime >= 5000) {
                    Serial.println("Emergency! Switching to FLICKER");
                    currentLedState = LED_FLICKER;
                    tempExceedTime = 0;
                }
            } else {
                Serial.println("Temperature normal, returning to SOLID");
                currentLedState = LED_SOLID;
                tempExceedTime = 0;
            }
            break;
            
        case LED_FLICKER:
            break;
    }
}

void setup() {
    ledInit();
    ledClear();
    ledFlush();
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // pinMode(TEMP_PIN, INPUT);
    
    analogReadResolution(12);

    // Increase ADC input range for better measurement
    analogSetPinAttenuation(TEMP_PIN, ADC_11db);
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("\nPerfectNoodle Kitchen Safety System");
    
    // Connect to Wi-Fi
    connectWiFi();
    
    if (connectServer()) {
        Serial.println("Flask server is online!");
        client.stop();
    } else {
        Serial.println("No flask server");
    }
    
    sendGetRequest();
    
    // local web server on ESP32 for debugging and monitoring
    // webServer.on("/", []() {
    //     String html = "<!DOCTYPE html><html>";
    //     html += "<head><title>ESP32 PerfectNoodle</title>";
    //     html += "<meta http-equiv='refresh' content='2'>";
    //     html += "</head><body>";
    //     html += "<h1>ESP32 PerfectNoodle</h1>";
    //     html += "<p>Temperature: " + String(readTemperatureC(), 1) + " °C</p>";
    //     html += "<p>LED State: ";
    //     switch(currentLedState) {
    //         case LED_SOLID: html += "SOLID (Normal)"; break;
    //         case LED_BLINK: html += "BLINK (Warning)"; break;
    //         case LED_CHASE: html += "CHASE (Alert)"; break;
    //         case LED_FLICKER: html += "FLICKER (Emergency)"; break;
    //     }
    //     html += "</p>";
    //     html += "<p>Threshold: " + String(TEMP_THRESHOLD) + " °C</p>";
    //     html += "</body></html>";
    //     webServer.send(200, "text/html", html);
    // });
    webServer.begin();
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    
    // Serial.println("System ready");
    // Serial.print("ESP32 IP address: ");
    // Serial.println(WiFi.localIP());
    // Serial.print("Flask server at: ");
    // Serial.print(serverHost);
    // Serial.print(":");
    // Serial.println(serverPort);
    // Serial.println("Browser: http://localhost:9194");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Handle incoming browser requests
    webServer.handleClient();
    
    // Serial.print("Wifi IP Address: ");
    // Serial.println(WiFi.localIP());

    //  static unsigned long lastTempMs = 0;
    // if (millis() - lastTempMs >= 2000) {
    //     printTemperature();
    //     lastTempMs = millis();
    // }

    // temperature read every 2 seconds
    if (currentTime - lastTempRead >= 2000) {
        float tempC = readTemperatureC();
        Serial.print("Temperature: ");
        Serial.print(tempC, 1);
        Serial.print(" °C | State: ");
        switch(currentLedState) {
            case LED_SOLID: Serial.print("SOLID"); break;
            case LED_BLINK: Serial.print("BLINK"); break;
            case LED_CHASE: Serial.print("CHASE"); break;
            case LED_FLICKER: Serial.print("FLICKER"); break;
        }
        Serial.println();
        lastTempRead = currentTime;
    }
    
    updatePatternByTemperature();
    
    // handle button press
    handleButton();
    
    // // Temperature exceeds threshold: switch to chase immediately, then flicker after 5s
    // static unsigned long tempExceedTime = 0;
    // if (currentLedState == LED_BLINK || currentLedState == LED_CHASE) {
    //     float tempC = readTemperatureC();
    //     if (tempC > TEMP_THRESHOLD) {
    //         if (tempExceedTime == 0) {
    //             tempExceedTime = millis();
    //             currentLedState = LED_CHASE;
    //             Serial.println("Temperature exceeded threshold — switching to chase, 5s timer started.");
    //         } else if (currentLedState == LED_CHASE && millis() - tempExceedTime >= 5000) {
    //             Serial.println("5s elapsed — switching to flicker.");
    //             currentLedState = LED_FLICKER;
    //             tempExceedTime = 0;
    //         }
    //     } else {
    //         if (tempExceedTime != 0) {
    //             Serial.println("Temperature dropped below threshold — timer reset, back to blink.");
    //             currentLedState = LED_BLINK;
    //             tempExceedTime = 0;
    //         }
    //     }
    // }

    switch (currentLedState) {
        case LED_SOLID:
            solidLED();
            break;
        case LED_BLINK:
            blinkLED();
            break;
        case LED_CHASE:
            chaseLED();
            break;
        case LED_FLICKER:
            flickerLED();
            break;
    }
    
    // send data to flask every 5 second
    if (currentTime - lastDataSend >= 5000) {
        sendDataToFlaskServer();
        lastDataSend = currentTime;
    }
    
    delay(10);
}