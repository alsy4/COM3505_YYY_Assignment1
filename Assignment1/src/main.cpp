#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <Main.h>

// ---------------- Wi-Fi credentials ----------------
const char* wifiSsid = "Bon Clay";
const char* wifiPassword = "passwords";


// ---------------- Server information ----------------
// IP address of the machine running the test server, please update these to match your Wi-Fi network
const char* serverHost = "172.20.10.12";
const int serverPort = 9194;


// ---------------- Student identification ----------------
const char* myEmail = "nasshaari1@sheffield.ac.uk";
const char* laptopMac = "d8:f3:bc:58:30:07";

const int RED_LED_PIN = 6;
const int YELLOW_LED_PIN = 9;
const int GREEN_LED_PIN = 12;

void blinkLED();
void chaseLED();
void flickerLED();
void solidLED();



// ---------------- Networking objects ----------------
// Client used to connect to the external server
WiFiClient client;

// Local HTTP server running on port 80
WebServer webServer(80);    

void solidLED() {
    Serial.printf("Setting LED solid");

    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
}

void blinkLED() {

    Serial.printf("setting LED_PIN HIGH HIGH...\n");
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(1000);   // blocking delay (what happens if the switch is pressed now?)

    Serial.printf("setting LED_PIN LOW...\n");
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(1000);
}

void chaseLED() {
    const int pins[] = { RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN };
    const int numPins = 3;
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
    const int pins[] = { RED_LED_PIN, YELLOW_LED_PIN, GREEN_LED_PIN };
    const int numPins = 3;
    const int flickerDuration = 2000;   // Total flicker duration in ms
    const int minDelay = 30;            // Minimum flicker interval in ms
    const int maxDelay = 150;           // Maximum flicker interval in ms

    Serial.println("Starting random LED flicker...");

    unsigned long startTime = millis();

    while (millis() - startTime < flickerDuration) {
        // Pick a random LED
        int randomPin = pins[random(0, numPins)];

        // Flicker it ON then OFF
        digitalWrite(randomPin, HIGH);
        delay(random(minDelay, maxDelay));
        digitalWrite(randomPin, LOW);
        delay(random(minDelay, maxDelay));
    }

    // Ensure all LEDs are off after flickering
    for (int i = 0; i < numPins; i++) {
        digitalWrite(pins[i], random(0,2));
    }

    Serial.println("Flicker complete.");
}


// ------------------------------------------------------------
// Connect ESP32 to Wi-Fi
// ------------------------------------------------------------
void connectWiFi()
{
    Serial. print("Connecting to Wi-Fi");

    WiFi.begin(wifiSsid, wifiPassword);

    unsigned long startTime = millis();

    // Wait until connected
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");

        // Restart if connection takes too long
        if (millis() - startTime > 20000)
        {
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
bool connectServer()
{
    Serial.print("Connecting to server ");
    Serial.print(serverHost);
    Serial.print(":");
    Serial.println(serverPort);

    // Attempt TCP connection
    if (!client.connect(serverHost, serverPort))
    {
        Serial.println("Connection failed!");
        return false;
    }

    Serial.println("Connected to server!");
    return true;
}



// ------------------------------------------------------------
// Send HTTP GET request containing email and MAC address
// ------------------------------------------------------------
void sendGetRequest()
{
    // Obtain unique MAC address of ESP32
    String mac = WiFi.macAddress();

    // Construct request URL    
    String url = "/Index.html?email=";
    url += myEmail;
    url += "&mac=";
    url += mac;

    Serial.print("Requesting URL: ");
    Serial.println(url);


    // Send HTTP GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + serverHost + "\r\n" +
                 "Connection: close\r\n\r\n");


    // Wait for server reply
    unsigned long timeout = millis();

    while (!client.available())
    {
        if (millis() - timeout > 5000)
        {
            Serial.println("Client timeout!");
            client.stop();
            return;
        }
    }


    // Print server response
    Serial.println("\n--- Server Response ---");

    while (client.available())
    {
        String line = client.readStringUntil('\n');
        Serial.println(line);
    }

    // Close connection
    client.stop();
    Serial.println("--- Connection Closed ---");
}




// ------------------------------------------------------------
// Arduino Setup
// Runs once at startup
// ------------------------------------------------------------
void setup()
{
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);

    Serial.begin(115200);
    delay(3000);

    // Connect to Wi-Fi network
    // connectWiFi();

    // // Connect to server
    // if (!connectServer())
    // {
    //     Serial.println("Could not connect to server, restarting...");
    //     delay(10000);
    //     ESP.restart();
    // }

    // // Send the GET request
    // sendGetRequest();


    // // --------------------------------------------------------
    // // Start local web server
    // // --------------------------------------------------------
    // webServer.on("/", []()
    // {
    //     webServer.send(200,
    //                    "text/html",
    //                    "<h1>ESP32 Ex08 Home Test</h1><p>Local page works!</p>");
    // });

    // webServer.begin();

    Serial.println("Local web server started");
}



// ------------------------------------------------------------
// Arduino Loop
// Runs continuously after setup
// ------------------------------------------------------------
void loop()
{
    // Handle incoming browser requests
    webServer.handleClient();
    Serial.print("Wifi IP Address: ");
    Serial.println(WiFi.localIP());
    flickerLED();
    // Remote server request only runs once in setup()

}   