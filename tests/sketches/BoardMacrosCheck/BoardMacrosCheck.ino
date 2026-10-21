#if !defined(ARDUINO_VTLABS_IOT_ESP32_R1) || !defined(VTLABS_BOARD_IOT_ESP32) || VTLABS_HW_REV != 1
#error "VTLabs board macros missing"
#endif
#include <Wire.h>
#include <SPI.h>
void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  Serial.printf("VTLabs rev %d, core %s, F_CPU %lu\n", VTLABS_HW_REV, ESP_ARDUINO_VERSION_STR, (unsigned long)F_CPU);
}
void loop() {
}
