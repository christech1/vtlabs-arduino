/*
  VTLabs IoT Training Kit - L3_04 I2C Address Scanner
  ------------------------------------------------------------------
  Purpose:     Find every device on the I2C bus and name the ones the kit
               knows about. First step whenever an I2C device "does not work".
  Hardware:    I2C bus (SDA/SCL) with whatever is fitted: OLED, keypad
               expander (PCF8574), BME280 module...
  Libraries:   none (Wire is part of the ESP32 core)
  Expected:    "Found 0x3C  OLED display (SSD1306)" etc., rescanned every 5 s.
  Experiments: 1. Unplug a module while running - does it disappear?
               2. Change an address jumper on a PCF8574 board and rescan.
               3. Look up why 0x00-0x07 and 0x78-0x7F are not scanned.
  Troubleshooting:
    - Nothing found: SDA/SCL swapped, no pull-up resistors, or no power to module.
    - Everything found at every address: SDA or SCL shorted to GND.
  @requires: none
*/
#include <VTLabsKit.h>
#include <Wire.h>

const char *knownName(uint8_t a) {
  if (a == VTL_I2C_OLED) return "OLED display (SSD1306)";
  if (a == VTL_I2C_BME280 || a == 0x77) return "BME280 sensor";
  if (a >= 0x20 && a <= 0x27) return "PCF8574 expander (keypad)";
  if (a >= 0x38 && a <= 0x3F && a != VTL_I2C_OLED) return "PCF8574A expander";
  if (a == 0x68) return "RTC DS3231 / MPU6050";
  return "unknown device";
}

void setup() {
  Serial.begin(115200);
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
}

void loop() {
  Serial.printf("Scanning I2C (SDA=%d, SCL=%d)...\n", VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  int found = 0;
  for (uint8_t addr = 0x08; addr < 0x78; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Found 0x%02X  %s\n", addr, knownName(addr));
      found++;
    }
  }
  Serial.printf("%d device(s)\n\n", found);
  delay(5000);
}
