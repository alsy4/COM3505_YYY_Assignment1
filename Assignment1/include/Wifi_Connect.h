#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// ---------------- Wi-Fi credentials ----------------
extern const char* wifiSsid;
extern const char* wifiPassword;

// ---------------- Server information ----------------
extern const char* serverHost;
extern const int   serverPort;

// ---------------- Student identification ----------------
extern const char* myEmail;
extern const char* laptopMac;

// Shared WiFi client (used by sendGetRequest)
extern WiFiClient client;

void connectWiFi();
bool connectServer();
void sendGetRequest();