// Ex03.cpp / Ex03.ino
// COM3505 – Exercise 03
//
// This exercise demonstrates:
//  - controlling multiple LEDs (red, yellow, green) to simulate a traffic light
//  - reading a switch to trigger changes
//  - sequencing LED states with delays
//  - using flags to maintain state between button presses

#include "Thing.h"  // include helper functions and previous definitions

// ---------------------------------------------------------------------------
// GPIO pin assignments for LEDs
// Assign pins to red, yellow, and green LEDs. These can be connected on a
// breadboard to visualize a traffic light sequence.
// ---------------------------------------------------------------------------

const int red    = 6;
const int yellow = 9;
const int green  = 12;

// ---------------------------------------------------------------------------
// Runtime state
// ON_RED keeps track of whether the traffic light is currently showing red.
// This allows changeLights() to know which state to transition to next.
// ---------------------------------------------------------------------------

bool ON_RED = false;

// Forward declaration
void changeLights();  // changes traffic light LEDs depending on current state

// ---------------------------------------------------------------------------
// setup03()
// Runs once at startup for Exercise 03. Sets up additional LEDs.
// ---------------------------------------------------------------------------

void setup03() {
  setup02();  // call previous setup() from Ex02 (serial, built-in LED, switch, etc.)
  
  Serial.println("\nsetup03...");  // print a startup banner for debugging

  // Configure the new LED pins as outputs
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  // Start the traffic light sequence with green on
  digitalWrite(green, HIGH);
}


// ---------------------------------------------------------------------------
// loop03()
// Runs repeatedly after setup03() completes.
// Checks the state of the push-button switch to trigger the traffic light.
// ---------------------------------------------------------------------------

void loop03() {
  // Read switch on pin 5 (INPUT_PULLUP style, LOW = pressed)
  if (digitalRead(5) == LOW) {
    Serial.printf(" ON_RED is %d; switch pressed...\n", ON_RED);

    // Change the traffic light LEDs according to the current state
    changeLights();
  }
}


// ---------------------------------------------------------------------------
// changeLights()
// Updates the traffic light sequence based on the current state (ON_RED).
// Implements simple timing using delay().
// ---------------------------------------------------------------------------

void changeLights() {
  if (ON_RED) {
    // If the light is currently red, transition to green

    // Turn on yellow as part of "red + yellow" combination for 2 seconds
    digitalWrite(yellow, HIGH);
    delay(2000);  // blocking delay to hold the LEDs on

    // Turn off red and yellow, then turn on green
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
  } else {
    // If the light is currently green, transition to red

    // Turn off green and turn yellow on for 3 seconds
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    delay(3000);  // blocking delay

    // Turn off yellow, then turn red on for 5 seconds
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
  }

  // Toggle the state flag so the next call transitions correctly
  ON_RED = !ON_RED;
}