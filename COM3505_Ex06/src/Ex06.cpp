// Ex06.cpp
// COM3505 – Exercise 06
// Exercise 06: WiFi Access Point and Web Server
// Objective:
// Learn how to configure the ESP32 as a WiFi access point and serve web pages to connected clients.
// Description: The ESP32 creates its own wireless network and runs a web server on port 80. Devices that connect to the network can
// open a browser and access the ESP32 via its IP address(typically 192.168.4.1). The server responds to HTTP
// requests and delivers a simple HTML page.

//Concepts demonstrated:
//- WiFi Access Point mode
//- Embedded web servers
//- Handling HTTP requests
//- Generating basic HTML responses

#include <Arduino.h>
#include "Thing.h"
#include <WiFi.h>
#include <WebServer.h>

/*
--------------------------------------------------
Global objects
--------------------------------------------------
*/

// Name of the WiFi Access Point the ESP will create
String apSSID = "COM3505-IoT-Ex06";

// Web server running on port 80 (standard HTTP port)
WebServer webServer(80);

// Loop counter for debugging / monitoring
unsigned long loopIteration = 0;


/*
--------------------------------------------------
Function declarations
--------------------------------------------------
*/

void startAP();
void startWebServer();
void handleRoot();
void handleNotFound();


/*
==================================================
Setup
==================================================
Runs once when the ESP starts.
*/
void setup()
{
  Serial.begin(115200);
  delay(500);

  startAP();        // Start the WiFi access point
  startWebServer(); // Start the HTTP server
}


/*
==================================================
Main loop
==================================================
Runs continuously after setup().
*/
void loop()
{
  loopIteration++;

  // Process incoming HTTP requests
  webServer.handleClient();
}


/*
==================================================
Start WiFi Access Point
==================================================
Creates a WiFi network that other devices can join.
*/
void startAP()
{
  WiFi.mode(WIFI_AP_STA);                 // Access point + station mode
  WiFi.softAP(apSSID.c_str(), "12345678"); // Password must be ≥ 8 chars

  Serial.println();
  Serial.println("Access Point started");
  Serial.print("SSID: ");
  Serial.println(apSSID);

  Serial.print("Connect to: http://");
  Serial.println(WiFi.softAPIP());        // Usually 192.168.4.1
}


/*
==================================================
Start Web Server
==================================================
Registers URLs and starts the server.
*/
void startWebServer()
{
  webServer.on("/", handleRoot);        // Homepage
  webServer.onNotFound(handleNotFound); // Unknown pages

  webServer.begin();

  Serial.println("Web server started");
}


/*
==================================================
Homepage Handler
==================================================
Called when a browser opens /
Example: http://192.168.4.1
*/
void handleRoot()
{
  String page =
    "<html>"
    "<head><title>COM3505 IoT</title></head>"
    "<body>"
    "<h2>Welcome to COM3505 Internet of Things</h2>"
    "<p>Your ESP32 web server is working!</p>"
    "</body>"
    "</html>";

  webServer.send(200, "text/html", page);
}


/*
==================================================
404 Handler
==================================================
Runs when a page does not exist.
*/
void handleNotFound()
{
  webServer.send(404, "text/plain", "404: Page not found");
}