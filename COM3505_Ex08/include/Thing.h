#pragma once

/**
 * ================================================================
 * Thing.h
 * ================================================================
 *
 * Description:
 * ------------
 * This header file declares global variables and configuration
 * flags shared across the project.
 *
 * It provides:
 *   • Debug control flags
 *   • Global loop counter
 *   • Device MAC address storage
 *
 * The actual definitions of these variables exist in a .cpp file.
 * This file only exposes them using `extern` so they can be shared
 * across multiple compilation units.
 *
 * ================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>


// ================================================================
// Debug Flags
// ================================================================

/**
 * startupDBG
 * -----------
 * Enables detailed debug output during system startup.
 * Used for printing initialization information such as:
 *   • SSID
 *   • IP addresses
 *   • Hardware configuration
 */
extern bool startupDBG;


/**
 * netDBG
 * ------
 * Enables additional network diagnostics.
 * When true, WiFi diagnostic information is printed
 * using WiFi.printDiag().
 */
extern bool netDBG;


// ================================================================
// Global Runtime Variables
// ================================================================

/**
 * loopIteration
 * -------------
 * Global counter incremented in the main loop().
 *
 * Used for:
 *   • Timing control
 *   • Throttling operations
 *   • Debugging loop behaviour
 */
extern unsigned long loopIteration;


/**
 * MAC_ADDRESS
 * -----------
 * Stores the device's MAC address as a String.
 *
 * Can be used for:
 *   • Device identification
 *   • Unique SSID generation
 *   • Debug output
 */
extern String MAC_ADDRESS;