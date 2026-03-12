// Ex02.ino / Ex02.cpp
// COM3505 – Exercise 02
//
// This exercise demonstrates:
//  - reading a digital input (a push-button switch)
//  - controlling an external LED as an actuator
//  - switching behaviour at runtime based on sensor input
//  - identifying the ESP32 using its MAC address// Ex02.ino/.cpp
//
//This example sketch is used in the COM3505 Internet of Things module (Academic Year 2025–2026) 
//at the University of Sheffield.
//The original version of this code was created by Prof. Hamish Cunningham.
//It has since been updated and motified by Prof. Po Yang for use in COM3505. 
//Ensuring compatibility with current ESP32 boards, Arduino IDE versions, and ESP-IDF releases.

#include "Thing.h"
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Runtime state
// This flag determines whether we blink the built-in LED or the external LED.
// It is toggled whenever the switch is pressed.
// ---------------------------------------------------------------------------

bool useInternalLED = true;   // start by blinking the built-in LED

// ---------------------------------------------------------------------------
// Pin definitions
// The GPIO numbering differs between the original ESP32 Feather and the
// ESP32-S3 Feather. We use conditional compilation so the same source code
// works correctly on both boards.
// ---------------------------------------------------------------------------

// Changes from ESP32 Feather to ESP32-S3:
//   pin 32 becomes 6
//   pin 14 becomes 5
#ifdef ARDUINO_FEATHER_ESP32
  #define LED_PIN     32
  #define SWITCH_PIN  14
#else
  #define LED_PIN      6
  #define SWITCH_PIN   5
#endif

// ---------------------------------------------------------------------------
// MAC address storage
// A MAC address is 6 bytes, represented as 12 hexadecimal characters.
// We add one extra byte for the null terminator.
// ---------------------------------------------------------------------------

char MAC_ADDRESS[13];   // "AABBCCDDEEFF\0"

// Forward declaration for a helper function defined later in the file
void macro_prints();    // print useful compile-time configuration info


// ---------------------------------------------------------------------------
// setup()
// Runs once at boot.
// Used to initialise serial I/O, identify the device, and configure GPIO.
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);           // initialise the serial line for debugging

  getMAC(MAC_ADDRESS);            // read and store the ESP32 MAC address
  pinMode(LED_BUILTIN, OUTPUT);   // configure built-in LED as output

  // Print a simple startup banner including the device ID
  Serial.printf("\nsetup02...\nESP32 MAC = %s\n", MAC_ADDRESS);

  // Configure GPIO pin connected to the external LED
  pinMode(LED_PIN, OUTPUT);       // digital output (on/off)

  // Configure GPIO pin connected to the push-button switch
  // INPUT_PULLUP enables the ESP32's internal pull-up resistor, so the
  // pin reads HIGH when unpressed and LOW when pressed.
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // Print compile-time and platform information for debugging
  macro_prints();
}

// ---------------------------------------------------------------------------
// getMAC()
// Reads the ESP32’s unique MAC address from eFuse memory and converts it
// into a human-readable string.
// ---------------------------------------------------------------------------

void getMAC(char *buf) {
  // The ESP32 MAC is stored as a 64-bit value
  uint64_t mac = ESP.getEfuseMac();

  // Convert to hex string (high 2 bytes + low 4 bytes)
  char rev[13];
  sprintf(rev, "%04X%08X",
          (uint16_t) (mac >> 32),
          (uint32_t) mac);

  // Reverse byte order to match standard MAC formatting
  for(int i = 0, j = 11; i <= 10; i += 2, j -= 2) {
    buf[i]     = rev[j - 1];
    buf[i + 1] = rev[j];
  }
  buf[12] = '\0';  // null-terminate the string
}

// ---------------------------------------------------------------------------
// loop()
// Runs repeatedly after setup() completes.
// Reads the switch state and controls LED behaviour accordingly.
// ---------------------------------------------------------------------------

void loop() {

  // Check if the switch is pressed (LOW because of INPUT_PULLUP)
  if(digitalRead(SWITCH_PIN) == LOW) {
    Serial.printf("switch is pressed...\n");

    // Toggle which LED we are controlling
    useInternalLED = !useInternalLED;
  }

  if(useInternalLED) {
    // Blink the built-in LED using the helper function from Thing.h
    Serial.printf("blinking internal LED...\n");
    blink(1, 500);
  } else {
    // Manually control the external LED
    Serial.printf("setting LED_PIN HIGH...\n");
    digitalWrite(LED_PIN, HIGH);
    delay(500);   // blocking delay (what happens if the switch is pressed now?)

    Serial.printf("setting LED_PIN LOW...\n");
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

// ---------------------------------------------------------------------------
// macro_prints()
// Prints useful compile-time macros to help diagnose build and platform
// configuration issues.
// Adapted from the HelloWorld example.
// ---------------------------------------------------------------------------

void macro_prints() {

  #ifdef ESP_IDF_VERSION_MAJOR
    Serial.printf(
      "IDF version: %d.%d.%d\n",
      ESP_IDF_VERSION_MAJOR,
      ESP_IDF_VERSION_MINOR,
      ESP_IDF_VERSION_PATCH
    );
  #endif

  #ifdef ESP_ARDUINO_VERSION_MAJOR
    Serial.printf(
      "ESP_ARDUINO_VERSION_MAJOR=%d; MINOR=%d; PATCH=%d\n",
      ESP_ARDUINO_VERSION_MAJOR,
      ESP_ARDUINO_VERSION_MINOR,
      ESP_ARDUINO_VERSION_PATCH
    );
  #endif

  #ifdef ARDUINO_FEATHER_ESP32
    Serial.printf("ARDUINO_FEATHER_ESP32 is defined\n");
  #endif

  #ifdef ARDUINO_ARCH_ESP32
    Serial.printf("ARDUINO_ARCH_ESP32 is defined\n");
  #endif

  #if CONFIG_IDF_TARGET_ESP32
    Serial.printf("CONFIG_IDF_TARGET_ESP32\n");
  #endif

  #if CONFIG_IDF_TARGET_ESP32S3
    Serial.printf("CONFIG_IDF_TARGET_ESP32S3\n");
  #endif

  #ifdef ESP_PLATFORM
    Serial.printf("ESP_PLATFORM is defined\n");
  #endif

  #ifdef ESP32
    Serial.printf("ESP32 is defined\n");
  #endif

  #ifdef IDF_VER
    Serial.printf("IDF_VER=%s\n", IDF_VER);
  #endif

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
}

// ---------------------------------------------------------------------------
// LED helper functions
// These wrap simple digitalWrite calls to make the main logic clearer.
// ---------------------------------------------------------------------------

void ledOn()  { digitalWrite(LED_BUILTIN, HIGH); }
void ledOff() { digitalWrite(LED_BUILTIN, LOW); }

// Blink the built-in LED a given number of times with a given delay
void blink(int times, int pause) {
  ledOff();
  for(int i = 0; i < times; i++) {
    ledOn();  delay(pause);
    ledOff(); delay(pause);
  }
}