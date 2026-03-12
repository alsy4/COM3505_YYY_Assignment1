// Ex04.cpp / Ex04.ino
// COM3505 – Exercise 04
//
// This exercise introduces a simple debugging infrastructure.
// Instead of printing messages directly using Serial.print(),
// we use debugging macros that allow messages to be turned
// on or off depending on the debug level.
//
// This approach is common in embedded systems because it:
// 1) keeps code clean
// 2) avoids unnecessary serial output
// 3) allows selective debugging for different parts of a program

#include "Thing.h"

// ---------------------------------------------------------------------------
// setup04()
// Runs once at startup.
// Calls previous setup functions and prints a debug startup message.
// ---------------------------------------------------------------------------
void setup04() {
  setup03();            
  // previous setups: get the MAC, set up GPIO pins, ...
  // Call setup03() from Exercise 03.
  // This ensures that all earlier configuration is performed:
  // - Serial communication started
  // - GPIO pins configured
  // - LEDs initialised
  // - MAC address retrieved

  dln(startupDBG, "\nsetup04..."); 
  // debug printout on serial, with newline
  // Debug message printed only if the debug level "startupDBG"
  // is enabled.
  //
  // dln = "debug line"
  // It behaves like Serial.println() but is controlled
  // by the debugging system defined in Thing.h.
  //
  // This helps developers track when the system boots.

}


// ---------------------------------------------------------------------------
// loop04()
// Runs continuously after setup finishes.
// Demonstrates periodic debug output and LED activity.
// ---------------------------------------------------------------------------
void loop04() {
  dbg(loopDBG, "ESP32 MAC = "); 
  // dbg() prints text WITHOUT a newline.
  // The message is printed only if the debug category
  // "loopDBG" is enabled.
  //
  // Here we start printing the ESP32's MAC address label.
  dln(loopDBG, MAC_ADDRESS);    
  // dln() prints the actual MAC address followed by a newline.
  //
  // MAC_ADDRESS was previously obtained during setup and
  // uniquely identifies this ESP32 board.
  blink(3);                     
  // blink the on-board LED.
  // It flashes the built-in LED three times.
  //
  // This provides visible feedback that the program loop
  // is running correctly.
  delay(1000);                  
  // Pause for 1 second before the loop repeats.
  //
  // This prevents the serial output from flooding the console
  // and makes the blinking pattern easier to observe.

}