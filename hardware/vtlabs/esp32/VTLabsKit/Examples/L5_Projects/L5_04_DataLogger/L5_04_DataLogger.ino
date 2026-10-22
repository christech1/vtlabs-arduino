/*
  VTLabs IoT Training Kit - L5_04 Project: Time-Stamped Data Logger
  ------------------------------------------------------------------
  Purpose:     A complete logger: real date/time from the internet (NTP),
               CSV in flash, start/stop with a button, status on the OLED,
               download over Wi-Fi.
  Hardware:    DHT22, LDR, OLED, BTN1 (start/stop), LED_GREEN (logging).
  Setup:       Wi-Fi in "arduino_secrets.h". TZ_INFO is set for Rwanda (CAT, UTC+2).
  Libraries:   "Adafruit SSD1306" + "Adafruit GFX Library" + "Adafruit BusIO",
               "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    OLED: time, LOGGING/STOPPED, number of records, IP address.
               /data.csv rows like "2026-10-22 14:05:00,24.8,52.0,61".
  Experiments: 1. Log only when a value changed by more than a set amount.
               2. Start a new file every day (name it by date).
               3. Compare with the SD-card version (L3_08). Pros and cons?
  Troubleshooting:
    - Time shows 1970: NTP not reached yet (needs internet), wait or check Wi-Fi.
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <time.h>
#include "arduino_secrets.h"

const char *TZ_INFO = "CAT-2";  // POSIX TZ string: Central Africa Time, UTC+2
const unsigned long INTERVAL_MS = 30000;
const char *FILE_NAME = "/data.csv";

WebServer server(80);
Adafruit_SSD1306 oled(VTL_OLED_WIDTH, VTL_OLED_HEIGHT, &Wire, -1);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
VTLButton btn;
bool logging = true;
unsigned long records = 0;

String timestamp() {
  struct tm now;
  if (!getLocalTime(&now, 10)) return "no-time";
  char buf[24];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &now);
  return buf;
}

void writeRecord() {
  float t = dht.readTemperature(), h = dht.readHumidity();
  if (isnan(t) || isnan(h)) return;
  bool header = !LittleFS.exists(FILE_NAME);
  File f = LittleFS.open(FILE_NAME, FILE_APPEND);
  if (!f) return;
  if (header) f.println("time,temperature_C,humidity_pct,light_pct");
  f.printf("%s,%.1f,%.1f,%d\n", timestamp().c_str(), t, h, (int)map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100));
  f.close();
  records++;
}

void showStatus() {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println(timestamp());
  oled.setTextSize(2);
  oled.println(logging ? "LOGGING" : "STOPPED");
  oled.setTextSize(1);
  oled.printf("records: %lu\n", records);
  oled.println(WiFi.localIP());
  oled.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  oled.begin(SSD1306_SWITCHCAPVCC, VTL_I2C_OLED);
  dht.begin();
  LittleFS.begin(true);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  configTzTime(TZ_INFO, "pool.ntp.org", "time.google.com");
  Serial.printf("\nLogger: http://%s/data.csv\n", WiFi.localIP().toString().c_str());

  server.on("/data.csv", HTTP_GET, []() {
    File f = LittleFS.open(FILE_NAME);
    if (!f) { server.send(404, "text/plain", "no data"); return; }
    server.streamFile(f, "text/csv");
    f.close();
  });
  server.begin();
}

void loop() {
  server.handleClient();
  btn.update();
  if (btn.pressed()) logging = !logging;
  digitalWrite(VTL_PIN_LED_GREEN, logging);

  static unsigned long lastLog = 0, lastShow = 0;
  if (logging && millis() - lastLog >= INTERVAL_MS) { lastLog = millis(); writeRecord(); }
  if (millis() - lastShow >= 1000) { lastShow = millis(); showStatus(); }
}
