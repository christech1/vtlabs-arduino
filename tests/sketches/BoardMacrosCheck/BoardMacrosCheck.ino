/*
  Test: board identification macros and Serial2 pin override.
  @requires: none
*/
#include <VTLabsKit.h>
#if !defined(ARDUINO_VTLABS_IOT_ESP32_R1) || !defined(VTLABS_BOARD) || VTLABS_HW_REV != 1 || VTL_HW_REV != 1
#error "VTLabs board macros missing"
#endif
// Espressif core 3.3.x defaults Serial2 to GPIO4/25 on ESP32 (our relay and red LED).
// The VTLabs variant must override them.
static_assert(RX2 == VTL_PIN_UART2_RX && TX2 == VTL_PIN_UART2_TX, "Serial2 pins must come from the VTLabs variant");
static_assert(LED_BUILTIN == VTL_PIN_LED_BUILTIN, "LED_BUILTIN must match the profile");
static_assert(SDA == VTL_PIN_I2C_SDA && SCL == VTL_PIN_I2C_SCL, "I2C pins must match the profile");
#include <Wire.h>
#include <SPI.h>
void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  vtlPrintBoardInfo(Serial);
}
void loop() {
}
