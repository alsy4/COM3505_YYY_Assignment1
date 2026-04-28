#include "Wifi_Connect.h"

// ---------------- Wi-Fi credentials ----------------
const char* wifiSsid     = "Bon Clay";
const char* wifiPassword = "passwords";

// ---------------- Server information ----------------
const char* serverHost = "172.20.10.12";
const int   serverPort = 9194;

// ---------------- Student identification ----------------
const char* myEmail   = "mdbmohdasri1@sheffield.ac.uk";
const char* laptopMac = "F0-A7-31-4B-C0-EE";

// Shared WiFi client
WiFiClient client;


// ------------------------------------------------------------
// Connect ESP32 to Wi-Fi
// ------------------------------------------------------------
void connectWiFi() {
    Serial.print("Connecting to Wi-Fi");

    WiFi.begin(wifiSsid, wifiPassword);

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");

        if (millis() - startTime > 20000) {
            Serial.println("\nFailed to connect to Wi-Fi!");
            ESP.restart();
        }
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");
    Serial.print("Device IP address: ");
    Serial.println(WiFi.localIP());
}


// ------------------------------------------------------------
// Connect to remote server
// ------------------------------------------------------------
bool connectServer() {
    Serial.print("Connecting to server ");
    Serial.print(serverHost);
    Serial.print(":");
    Serial.println(serverPort);

    if (!client.connect(serverHost, serverPort)) {
        Serial.println("Connection failed!");
        return false;
    }

    Serial.println("Connected to server!");
    return true;
}


// ------------------------------------------------------------
// Send HTTP GET request containing email and MAC address
// ------------------------------------------------------------
void sendGetRequest() {
    String mac = WiFi.macAddress();

    String url = "/Index.html?email=";
    url += myEmail;
    url += "&mac=";
    url += mac;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + serverHost + "\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();

    while (!client.available()) {
        if (millis() - timeout > 5000) {
            Serial.println("Client timeout!");
            client.stop();
            return;
        }
    }

    Serial.println("\n--- Server Response ---");

    while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
    }

    client.stop();
    Serial.println("--- Connection Closed ---");
}