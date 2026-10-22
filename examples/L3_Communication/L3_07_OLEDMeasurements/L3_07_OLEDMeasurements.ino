/*
  VTLabs IoT Training Kit - L3_07 Show Measurements on the OLED Display
  ------------------------------------------------------------------
  Purpose:     Draw text and a bar graph on an I2C OLED (SSD1306).
  Hardware:    SSD1306 128x64 OLED at VTL_I2C_OLED (0x3C), POT, LDR, DHT22.
  Libraries:   "Adafruit SSD1306" (>= 2.5) + "Adafruit GFX Library" + "Adafruit BusIO"
               "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    Screen shows temperature, humidity, light %, and a bar that
               follows the potentiometer. Updates every 0.5 s.
  Experiments: 1. Add a second page selected with BTN1.
               2. Draw a small graph of the last 64 light readings.
               3. Invert the display when the switch is ON.
  Troubleshooting:
    - "OLED not found": run L3_04. Some 1.3" displays use SH1106, which needs
      a different library (Adafruit SH110X).
    - Display shows noise: wrong height (128x32 vs 128x64) in the profile.
  @requires: none
*/
#include <VTLabsKit.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

Adafruit_SSD1306 oled(VTL_OLED_WIDTH, VTL_OLED_HEIGHT, &Wire, -1);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
float t = NAN, h = NAN;

void setup() {
  Serial.begin(115200);
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, VTL_I2C_OLED)) {
    Serial.println("OLED not found");
    while (true) delay(1000);
  }
  dht.begin();
}

void loop() {
  static unsigned long lastDht = 0;
  if (millis() - lastDht > 2500) {
    lastDht = millis();
    t = dht.readTemperature();
    h = dht.readHumidity();
  }
  int light = map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100);
  int pot = analogRead(VTL_PIN_POT);

  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("VTLabs IoT Kit");
  oled.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  oled.setCursor(0, 16);
  if (isnan(t)) oled.println("Temp: --");
  else oled.printf("Temp: %.1f C\nHum:  %.0f %%\n", t, h);
  oled.setCursor(0, 36);
  oled.printf("Light: %d %%", light);
  oled.drawRect(0, 50, 128, 12, SSD1306_WHITE);
  oled.fillRect(2, 52, map(pot, 0, 4095, 0, 124), 8, SSD1306_WHITE);
  oled.display();
  delay(500);
}
