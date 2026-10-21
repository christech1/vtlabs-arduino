/*
 * VTLabs IoT Training Kit ESP32 - Rev 1 - Arduino variant
 *
 * This file provides the standard Arduino pin names (TX, SDA, SCK, A0, ...)
 * that the Espressif core and libraries expect from every variant.
 *
 * Trainer-specific peripherals (LEDs, buttons, relay, buzzer, display...) are
 * NOT defined here. They live in vtlabs_board.h once the schematic is verified
 * (Stage 4). Until then, only ESP32 facts and dev-module conventions appear.
 *
 * Base: espressif/arduino-esp32 3.3.12 variants/esp32/pins_arduino.h
 */
#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "esp_arduino_version.h"

/* ---- Supported Espressif core range ------------------------------------
 * The Boards Manager cannot make vtlabs:esp32 depend on esp32:esp32, so the
 * check happens here. Widen the range only after re-running the test suite.
 */
#define VTLABS_CORE_MIN ESP_ARDUINO_VERSION_VAL(3, 3, 0)
#define VTLABS_CORE_MAX_EXCL ESP_ARDUINO_VERSION_VAL(3, 4, 0)
#if (ESP_ARDUINO_VERSION < VTLABS_CORE_MIN) || (ESP_ARDUINO_VERSION >= VTLABS_CORE_MAX_EXCL)
#error "VTLabs IoT Training Kit needs the 'esp32 by Espressif Systems' platform version 3.3.x. Install it from Tools > Board > Boards Manager."
#endif

/* ---- On-board LED -------------------------------------------------------
 * ASSUMPTION A5: GPIO2, the usual blue LED on ESP32 DevKit-style modules.
 * Verify on the real board before release.
 */
static const uint8_t LED_BUILTIN = 2;
#define BUILTIN_LED LED_BUILTIN  // backward compatibility
#define LED_BUILTIN LED_BUILTIN  // allow testing #ifdef LED_BUILTIN

/* ---- UART0 (USB serial: programming + Serial Monitor) ------------------- */
static const uint8_t TX = 1;
static const uint8_t RX = 3;

/* ---- Default I2C (Wire) ------------------------------------------------- */
static const uint8_t SDA = 21;
static const uint8_t SCL = 22;

/* ---- Default SPI (VSPI) ------------------------------------------------- */
static const uint8_t SS = 5;
static const uint8_t MOSI = 23;
static const uint8_t MISO = 19;
static const uint8_t SCK = 18;

/* ---- ADC-capable pins (Arduino aliases, same as Espressif generic) -----
 * Note: ADC2 pins (A10..A19) cannot be read while Wi-Fi is active.
 */
static const uint8_t A0 = 36;
static const uint8_t A3 = 39;
static const uint8_t A4 = 32;
static const uint8_t A5 = 33;
static const uint8_t A6 = 34;
static const uint8_t A7 = 35;
static const uint8_t A10 = 4;
static const uint8_t A11 = 0;
static const uint8_t A12 = 2;
static const uint8_t A13 = 15;
static const uint8_t A14 = 13;
static const uint8_t A15 = 12;
static const uint8_t A16 = 14;
static const uint8_t A17 = 27;
static const uint8_t A18 = 25;
static const uint8_t A19 = 26;

/* ---- Touch pins ---------------------------------------------------------- */
static const uint8_t T0 = 4;
static const uint8_t T1 = 0;
static const uint8_t T2 = 2;
static const uint8_t T3 = 15;
static const uint8_t T4 = 13;
static const uint8_t T5 = 12;
static const uint8_t T6 = 14;
static const uint8_t T7 = 27;
static const uint8_t T8 = 33;
static const uint8_t T9 = 32;

/* ---- True DAC outputs ---------------------------------------------------- */
static const uint8_t DAC1 = 25;
static const uint8_t DAC2 = 26;

#endif /* Pins_Arduino_h */
