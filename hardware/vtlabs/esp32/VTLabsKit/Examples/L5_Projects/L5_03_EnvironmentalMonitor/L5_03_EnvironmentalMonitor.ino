/*
  VTLabs IoT Training Kit - L5_03 Project: Environmental Monitor (offline)
  ------------------------------------------------------------------
  Purpose:     A stand-alone monitor: several sensors, min/max statistics,
               display pages, and alarm indication with LEDs.
  Hardware:    DHT22, LDR, PIR, OLED, LED_GREEN/YELLOW/RED, BTN1 (next page),
               BTN2 (reset min/max).
  Libraries:   "Adafruit SSD1306" + "Adafruit GFX Library" + "Adafruit BusIO",
               "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    Page 1: current values. Page 2: min/max since reset.
               Page 3: alarm status. Green = OK, yellow = warning
               (T > 28 C or RH > 70 %), red = alarm (T > 32 C or RH > 85 %).
  Experiments: 1. Add a "comfort index" from temperature and humidity.
               2. Save min/max in Preferences so they survive a reset.
               3. Make the alarm limits adjustable with the POT.
  Troubleshooting:
    - Blank screen: run L3_04 and L3_07 first.
  @requires: none
*/
#include <VTLabsKit.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

Adafruit_SSD1306 oled(VTL_OLED_WIDTH, VTL_OLED_HEIGHT, &Wire, -1);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
VTLButton nextBtn, resetBtn;
float t = NAN, h = NAN, tMin = 99, tMax = -99, hMin = 101, hMax = -1;
int page = 0, level = 0;  // level 0 ok, 1 warning, 2 alarm
unsigned long motions = 0;

void resetStats() { tMin = 99; tMax = -99; hMin = 101; hMax = -1; motions = 0; }

void draw() {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.printf("ENV MONITOR  p%d/3\n---------------------\n", page + 1);
  if (page == 0) {
    oled.printf("Temp  %5.1f C\nHum   %5.1f %%\nLight %5d %%\nMotion %lu x", t, h,
                (int)map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100), motions);
  } else if (page == 1) {
    oled.printf("T min/max %.1f/%.1f\nH min/max %.0f/%.0f\n\nBTN2 = reset", tMin, tMax, hMin, hMax);
  } else {
    oled.setTextSize(2);
    oled.println(level == 0 ? "OK" : level == 1 ? "WARNING" : "ALARM!");
    oled.setTextSize(1);
  }
  oled.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  oled.begin(SSD1306_SWITCHCAPVCC, VTL_I2C_OLED);
  dht.begin();
  pinMode(VTL_PIN_PIR, VTL_PIN_PIR_MODE);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  nextBtn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  resetBtn.begin(VTL_PIN_BTN2, VTL_PIN_BTN2_ACTIVE_LOW, VTL_PIN_BTN2_MODE);
}

void loop() {
  nextBtn.update();
  resetBtn.update();
  if (nextBtn.pressed()) page = (page + 1) % 3;
  if (resetBtn.pressed()) resetStats();

  static int lastPir = LOW;
  int pir = digitalRead(VTL_PIN_PIR);
  if (pir == HIGH && lastPir == LOW) motions++;
  lastPir = pir;

  static unsigned long lastRead = 0;
  if (millis() - lastRead > 2500) {
    lastRead = millis();
    float nt = dht.readTemperature(), nh = dht.readHumidity();
    if (!isnan(nt) && !isnan(nh)) {
      t = nt; h = nh;
      tMin = min(tMin, t); tMax = max(tMax, t);
      hMin = min(hMin, h); hMax = max(hMax, h);
      level = (t > 32 || h > 85) ? 2 : (t > 28 || h > 70) ? 1 : 0;
      Serial.printf("T=%.1f H=%.1f level=%d\n", t, h, level);
    }
  }
  digitalWrite(VTL_PIN_LED_GREEN, level == 0);
  digitalWrite(VTL_PIN_LED_YELLOW, level == 1);
  digitalWrite(VTL_PIN_LED_RED, level == 2 && (millis() / 250) % 2);  // blink on alarm

  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 300) { lastDraw = millis(); draw(); }
}
