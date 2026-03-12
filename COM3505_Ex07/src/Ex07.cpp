// Ex07.cpp
// COM3505 – Exercise 06
// Exercise 07 – Simple HTML Utilities & Web Server (ESP32)
// Objective:
//This exercise demonstrates how to create simple utilities for generating HTML pages programmatically on an ESP32 device.
// Instead of storing large static HTML strings, we:
// Represent HTML elements using lightweight structures
// Build pages dynamically using reusable utilities
// Serve generated pages through a WebServer

 // The device:
 //   1. Starts in Wi-Fi Access Point (AP) mode
 //   2. Launches a web server on port 80
 //   3. Dynamically generates and serves an HTML page at "/"
 //
 // This approach:
 //   - Improves readability
 //   - Avoids long hard-coded HTML strings
 //   - Encourages modular, reusable page components
 //   - Demonstrates simple templating concepts


#include <Arduino.h>
#include "Thing.h"
#include "Ex07.h"

#include <WiFi.h>
#include <WebServer.h>

#define startupDBG true
#define netDBG true

// ================================================================
// Wi-Fi & Web Server Globals
// ================================================================

// SSID used for the Access Point
String apSSID;

// Create HTTP server on port 80
WebServer webServer(80);

// Used to throttle server polling inside loop()
unsigned long loopIteration = 0;


// ================================================================
// Function Prototypes
// ================================================================

void startAP();
void startWebServer();
void handleRoot();
void handleNotFound();
void printIPs();


// ================================================================
// Access Point Setup
// ================================================================

/**
 * startAP()
 *
 * Configures the ESP32 to operate in Access Point mode.
 * Creates a Wi-Fi network with a predefined SSID.
 */
void startAP() {
  apSSID = "COM3505-IoT-Ex07";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID.c_str());

  Serial.print("AP SSID: ");
  Serial.print(apSSID);
  Serial.print(" | AP IP: ");
  Serial.println(WiFi.softAPIP());
}


// ================================================================
// Arduino Setup
// ================================================================

void setup() {
  setup07();
}

/**
 * setup07()
 *
 * Initialises serial communication, Wi-Fi AP,
 * and starts the web server.
 */
void setup07() {

  Serial.begin(115200);
  delay(500);   // Small delay to stabilise serial monitor

  startAP();
  startWebServer();
}


// ================================================================
// Arduino Loop
// ================================================================

void loop() {
  loop07();
  printIPs();
}

/**
 * loop07()
 *
 * Repeatedly checks for incoming HTTP client requests.
 * Server handling is throttled using loopIteration
 * to reduce overhead in tight loops.
 */
void loop07() {

  loopIteration++;

  // Only check for web clients periodically
  if (loopIteration % 1000 == 0) {
    webServer.handleClient();
  }
}


// ================================================================
// Simple HTML Utility System
// ================================================================

/**
 * HTMLElement
 *
 * Represents a simple HTML element consisting of:
 *   • tag name
 *   • inner content
 *
 * Provides a render() function to convert
 * the element into a valid HTML string.
 */
struct HTMLElement {
  String tag;
  String content;

  String render() const {
    return "<" + tag + ">" + content + "</" + tag + ">";
  }
};

/**
 * H2()
 *
 * Helper function to create an <h2> element.
 */
HTMLElement H2(const String& text) {
  return {"h2", text};
}

/**
 * P()
 *
 * Helper function to create a <p> (paragraph) element.
 */
HTMLElement P(const String& text) {
  return {"p", text};
}


/**
 * Page Class
 *
 * Represents a full HTML document.
 *
 * Responsibilities:
 *   • Store page title
 *   • Accumulate body content
 *   • Render complete HTML page structure
 *
 * Demonstrates a very simple templating approach.
 */
class Page {
  String title;
  String body;

public:
  // Constructor sets page title
  Page(const String& t) : title(t) {}

  /**
   * add()
   *
   * Adds a rendered HTML element to the page body.
   */
  void add(const HTMLElement& e) {
    body += e.render();
    body += "\n";
  }

  /**
   * render()
   *
   * Returns the complete HTML document string.
   */
  String render() const {
    return
      "<html><head>"
      "<title>" + title + "</title>"
      "<meta charset='utf-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "</head><body>\n"
      + body +
      "\n</body></html>";
  }
};


// ================================================================
// HTTP Route Handlers
// ================================================================

/**
 * handleRoot()
 *
 * Handles requests to "/"
 *
 * Dynamically builds a page using:
 *   • Page class
 *   • H2 helper
 *   • P helper
 *
 * Sends the generated HTML to the client.
 */
void handleRoot() {

  // Create page using AP name as title
  Page page(apSSID);

  page.add(H2("Welcome to COM3505 Internet of Thing"));
  page.add(P("This page was created using Ex07 HTML utilities"));

  webServer.send(200, "text/html", page.render());
}


/**
 * handleNotFound()
 *
 * Handles unknown routes.
 * Returns a simple plain text response.
 */
void handleNotFound() {
  webServer.send(200, "text/plain", "Page not found");
}


// ================================================================
// Debug Utilities
// ================================================================

/**
 * printIPs()
 *
 * Prints network information for debugging.
 * Uses debug flags defined elsewhere.
 */
void printIPs() {

  if(startupDBG) {
    Serial.print("AP SSID: ");
    Serial.print(apSSID);
    Serial.print("; IP address(es): local=");
    Serial.print(WiFi.localIP());
    Serial.print("; AP=");
    Serial.println(WiFi.softAPIP());
  }

  if(netDBG)
    WiFi.printDiag(Serial);
}


// ================================================================
// Web Server Initialisation
// ================================================================

/**
 * startWebServer()
 *
 * Registers URL handlers and starts the HTTP server.
 */
void startWebServer() {

  // Route for homepage
  webServer.on("/", handleRoot);

  // Catch-all route
  webServer.onNotFound(handleNotFound);

  webServer.begin();

  Serial.println("HTTP server started");
}