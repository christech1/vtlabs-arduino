/*
  VTLabs IoT Training Kit - L4_05 Log Measurements to Flash (LittleFS)
  ------------------------------------------------------------------
  Purpose:     Store measurements in the ESP32's own flash file system and
               download them as a CSV file from the browser.
  Hardware:    DHT22, LDR (on board).
  Setup:       Enter Wi-Fi details in the "arduino_secrets.h" tab.
               Tools > Partition Scheme: "Default 4MB with spiffs" (has a data area).
  Libraries:   "DHT sensor library" + "Adafruit Unified Sensor" (LittleFS is in the core)
  Expected:    A line "seconds,temperature,humidity,light" every 10 s in /log.csv.
               http://<IP>/          status page
               http://<IP>/log.csv   download (open in a spreadsheet)
               http://<IP>/clear     delete the log (POST from the page button)
               The log survives a reset or power cut.
  Experiments: 1. Change the interval to 60 s. How many days fit in the free space?
               2. Stop logging when the file is bigger than 100 kB.
               3. Plot the CSV in a spreadsheet.
  Troubleshooting:
    - "LittleFS mount failed": choose a partition scheme with a file system.
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <DHT.h>
#include "arduino_secrets.h"

const unsigned long LOG_INTERVAL_MS = 10000;
const char *LOG_FILE = "/log.csv";
WebServer server(80);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);

void logOnce() {
  float t = dht.readTemperature(), h = dht.readHumidity();
  int light = map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100);
  bool newFile = !LittleFS.exists(LOG_FILE);
  File f = LittleFS.open(LOG_FILE, FILE_APPEND);
  if (!f) { Serial.println("cannot open log"); return; }
  if (newFile) f.println("seconds,temperature_C,humidity_pct,light_pct");
  f.printf("%lu,%.1f,%.1f,%d\n", millis() / 1000, t, h, light);
  f.close();
  Serial.printf("logged: %.1f C %.1f %% %d %%\n", t, h, light);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  if (!LittleFS.begin(true)) Serial.println("LittleFS mount failed");  // true = format if needed

  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.printf("\nLogger: http://%s/\n", WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, []() {
    File f = LittleFS.open(LOG_FILE);
    size_t size = f ? f.size() : 0;
    if (f) f.close();
    server.send(200, "text/html",
                "<meta name=viewport content='width=device-width'><h2>VTLabs data logger</h2><p>Log size: " +
                    String(size) + " bytes. Free: " + String(LittleFS.totalBytes() - LittleFS.usedBytes()) +
                    " bytes.</p><a href=/log.csv>Download log.csv</a><form method=post action=/clear>"
                    "<button>Clear log</button></form>");
  });
  server.on("/log.csv", HTTP_GET, []() {
    File f = LittleFS.open(LOG_FILE);
    if (!f) { server.send(404, "text/plain", "no data yet"); return; }
    server.streamFile(f, "text/csv");
    f.close();
  });
  server.on("/clear", HTTP_POST, []() {
    LittleFS.remove(LOG_FILE);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  static unsigned long last = 0;
  if (millis() - last >= LOG_INTERVAL_MS) {
    last = millis();
    logOnce();
  }
}
