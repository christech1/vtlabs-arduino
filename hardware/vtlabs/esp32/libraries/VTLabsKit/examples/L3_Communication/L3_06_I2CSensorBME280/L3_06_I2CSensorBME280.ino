/*
  VTLabs IoT Training Kit - L3_06 I2C Sensor: BME280 (OPTIONAL MODULE)
  ------------------------------------------------------------------
  Purpose:     Talk to a sensor over I2C: address, init check, readings.
  Hardware:    OPTIONAL BME280 module (3.3 V version!) on the I2C header:
               VCC->3V3, GND->GND, SDA->SDA, SCL->SCL. Address VTL_I2C_BME280
               (0x76; some modules use 0x77 - the sketch tries both).
               Run L3_04 first to confirm the module is found.
  Libraries:   "Adafruit BME280 Library" (>= 2.2) + "Adafruit Unified Sensor"
               + "Adafruit BusIO"
  Expected:    "T 24.81 C  P 1013.2 hPa  RH 48.3 %  altitude 1550 m".
               Without the module: "BME280 not found" (no crash).
  Experiments: 1. Set SEA_LEVEL_HPA to today's value for Kigali and compare
                  the altitude with the real one (~1550 m).
               2. Compare temperature with the DHT22 (L3_05).
  Troubleshooting:
    - Not found: run L3_04; a BMP280 (no humidity) has chip ID 0x58 and
      will not work with this library.
  @requires: none
*/
#include <VTLabsKit.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

const float SEA_LEVEL_HPA = 1013.25;
Adafruit_BME280 bme;
bool ok = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  ok = bme.begin(VTL_I2C_BME280, &Wire) || bme.begin(0x77, &Wire);
  Serial.println(ok ? "BME280 ready" : "BME280 not found - optional module not fitted?");
}

void loop() {
  if (ok) {
    Serial.printf("T %.2f C  P %.1f hPa  RH %.1f %%  altitude %.0f m\n", bme.readTemperature(),
                  bme.readPressure() / 100.0F, bme.readHumidity(), bme.readAltitude(SEA_LEVEL_HPA));
  }
  delay(2000);
}
