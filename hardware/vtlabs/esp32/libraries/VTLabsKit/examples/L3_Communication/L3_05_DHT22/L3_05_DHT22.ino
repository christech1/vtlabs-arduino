/*
  VTLabs IoT Training Kit - L3_05 Temperature and Humidity (DHT22)
  ------------------------------------------------------------------
  Purpose:     Read a sensor that uses its own single-wire digital protocol,
               and handle failed readings properly.
  Hardware:    DHT22 on VTL_PIN_DHT (on board, with 10k pull-up).
  Libraries:   "DHT sensor library" by Adafruit (>= 1.4.6)
               + "Adafruit Unified Sensor" (installed automatically with it)
  Expected:    Every 2.5 s: "T = 24.6 C  RH = 51.2 %  heat index = 25.0 C".
               A failed read prints "read failed" instead of a wrong number.
  Experiments: 1. Breathe on the sensor - what changes first, T or RH?
               2. Read faster than every 2 s. What goes wrong? (DHT22 limit)
               3. Light LED_RED when T > 30 C.
  Troubleshooting:
    - Always "read failed": wrong pin/type, or missing pull-up resistor.
    - Library not found: Tools > Manage Libraries, install "DHT sensor library".
  @requires: none
*/
#include <VTLabsKit.h>
#include <DHT.h>

DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  delay(2500);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("read failed - check wiring and sensor type");
    return;
  }
  Serial.printf("T = %.1f C  RH = %.1f %%  heat index = %.1f C\n", t, h, dht.computeHeatIndex(t, h, false));
}
