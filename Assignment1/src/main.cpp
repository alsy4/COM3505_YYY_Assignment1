#include <Arduino.h>
#include <WebServer.h>
#include <Main.h>

#include "LED.h"
#include "Wifi_Connect.h"

// Local HTTP server running on port 80
WebServer webServer(80);


void setup() {
    pinMode(RED_LED_PIN,    OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN,  OUTPUT);

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

    Serial.println("Local web server started");
}


// ------------------------------------------------------------
// Arduino Loop — runs continuously after setup
// ------------------------------------------------------------
void loop() {
    // Handle incoming browser requests
    webServer.handleClient();

    Serial.print("Wifi IP Address: ");
    Serial.println(WiFi.localIP());

    chaseLED();

}