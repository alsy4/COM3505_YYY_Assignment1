
// sketch.ino
// Main entry point for an ESP32 Arduino sketch.
//
// This example demonstrates:
//  - Basic Arduino setup/loop structure
//  - Serial output for debugging
//  - Detection and reporting of ESP-IDF and Arduino core versions
//  - Conditional compilation based on board, architecture, and target
//  - Optional LED blinking as a simple hardware test
//
//This example sketch is used in the COM3505 Internet of Things module (Academic Year 2025–2026) 
//at the University of Sheffield.
//The original version of this code was created by Prof. Hamish Cunningham.
//It has since been updated and motified by Prof. Po Yang for use in COM3505. 
//Ensuring compatibility with current ESP32 boards, Arduino IDE versions, and ESP-IDF releases.

#include <Arduino.h>     // Core Arduino definitions
#include <esp_log.h>     // ESP-IDF logging support (not used directly here,
                         // but included to show IDF availability)


// to enable or disable LED blinking
bool doBlinking = false;
int pushButton = 5;
int externalLed = 6;

void setup() {
  // Initialise serial communication for debugging output
  Serial.begin(115200);
  Serial.println("arduino started");

  // Configure the built-in LED pin as an output
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(externalLed, OUTPUT);
} // setup

void loop() {
  // Print a simple greeting to show the sketch is running
  Serial.printf("\nahem, hello (merged bin) world\n");
  printMacAddress();

  // Print ESP-IDF version numbers if available
  #ifdef ESP_IDF_VERSION_MAJOR
    Serial.printf( // IDF version
      "IDF version: %d.%d.%d\n",
      ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH
    );
  #endif

  // Print Arduino ESP32 core version numbers if defined
  #ifdef ESP_ARDUINO_VERSION_MAJOR
    Serial.printf(
      "ESP_ARDUINO_VERSION_MAJOR=%d; MINOR=%d; PATCH=%d\n",
      ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR,
      ESP_ARDUINO_VERSION_PATCH
    );
  #endif

  // Board- and architecture-specific compile-time checks
  #ifdef ARDUINO_FEATHER_ESP32
    Serial.printf("ARDUINO_FEATHER_ESP32 is defined\n");
  #endif
  #ifdef ARDUINO_ARCH_ESP32
    Serial.printf("ARDUINO_ARCH_ESP32 is defined\n");
  #endif

  // Target-specific checks (useful when supporting multiple ESP32 variants)
  #if CONFIG_IDF_TARGET_ESP32
    Serial.printf("CONFIG_IDF_TARGET_ESP32\n");
  #endif
  #if CONFIG_IDF_TARGET_ESP32S3
    Serial.printf("CONFIG_IDF_TARGET_ESP32S3\n");
  #endif

  // General ESP platform macros
  #ifdef ESP_PLATFORM
    Serial.printf("ESP_PLATFORM is defined\n");
  #endif
  #ifdef ESP32
    Serial.printf("ESP32 is defined\n");
  #endif

   // Print IDF version string if provided
  #ifdef IDF_VER
    Serial.printf("IDF_VER=%s\n", IDF_VER);
  #endif

  // Arduino IDE build/version information
  #ifdef ARDUINO
    Serial.printf("ARDUINO=%d\n", ARDUINO);
  #endif
  #ifdef ARDUINO_BOARD
    Serial.printf("ARDUINO_BOARD=%s\n", ARDUINO_BOARD);
  #endif
  #ifdef ARDUINO_VARIANT
    Serial.printf("ARDUINO_VARIANT=%s\n", ARDUINO_VARIANT);
  #endif
  #ifdef ARDUINO_SERIAL_PORT
    Serial.printf("ARDUINO_SERIAL_PORT=%d\n", ARDUINO_SERIAL_PORT);
  #endif

  // Optional LED blinking as a basic hardware sanity check
  
  if(doBlinking) {
    digitalWrite(externalLed, HIGH);
    delay(2000);
    digitalWrite(externalLed, LOW);
    delay(2000);
  }
  
  readFromSwitch();
  delay(500);
} // loop

void printMacAddress() {
  uint64_t chipId = ESP.getEfuseMac();

  uint8_t mac[6];
  for (int i = 0; i < 6; i++) {
    mac[i] = (chipId >> (i * 8)) & 0xFF;
  }
  
  Serial.printf("Formatted MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);  
}

void readFromSwitch() {
  int buttonState = digitalRead(pushButton);
  Serial.print("Button Value: ");
  Serial.println(buttonState);
  if (buttonState == LOW) {
    digitalWrite(externalLed, HIGH);
    Serial.println("LED ON");
  } else {
    Serial.println("LED OFF");
    digitalWrite(externalLed, LOW);
  }
}