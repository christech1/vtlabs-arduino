/*
  VTLabs IoT Training Kit - L9_07 Exercise 7: IoT Monitor with Matrix Status
  ------------------------------------------------------------------
  Purpose:     A networked monitoring node: measurements on a web page and
               JSON API, a limit settable from the web (and the keypad if
               fitted), and LOCAL status on the LED matrix so people in the
               room see the state without a phone.
  Hardware:    LED matrix add-on, DHT22, LDR, BUZZER. OPTIONAL: keypad (A + value + #
               sets the limit locally).
  Setup:       Wi-Fi in the "arduino_secrets.h" tab.
  Libraries:   see L8_01, plus "DHT sensor library" + "Adafruit Unified Sensor"
  Matrix:      Wi-Fi icon (blinking) = connecting   check = OK, Wi-Fi connected
               warning (blinking) = temperature above limit
               cross = sensor fault   every 15 s the temperature scrolls by.
  Web:         http://<IP>/ page, /api/status JSON, POST /api/limit?c=30
  Expected:    Matrix and web page always agree on the state.
  Task:        Publish /api/status to MQTT as well (see L4_06).
  @requires: matrix
  @optional: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include "arduino_secrets.h"
#if VTL_HAS_KEYPAD
#include <VTLabsKeypad.h>
VTLKeypad keypad;
VTLLineInput entry;
bool editing = false;
#endif

VTLMatrix matrix;
WebServer server(80);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
float t = NAN, h = NAN, limitC = 30;
enum Status { ST_CONNECTING, ST_OK, ST_HOT, ST_FAULT };
const char *statusName[] = {"connecting", "ok", "over-limit", "sensor-fault"};

Status status() {
  if (WiFi.status() != WL_CONNECTED) return ST_CONNECTING;
  if (isnan(t)) return ST_FAULT;
  return t > limitC ? ST_HOT : ST_OK;
}

void drawStatus() {
  static unsigned long lastScroll = 0;
  if (matrix.scrolling()) { matrix.scrollUpdate(); return; }
  if (millis() - lastScroll > 15000 && !isnan(t)) {
    lastScroll = millis();
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1fC", t);
    matrix.setColor(VTL_CYAN);
    matrix.scrollText(buf);
    return;
  }
  Status s = status();
  bool blink = (millis() / 300) % 2;
  const uint8_t *icon[] = {vtl_icons::WIFI, vtl_icons::CHECK, vtl_icons::WARNING, vtl_icons::CROSS};
  const uint32_t col[] = {VTL_BLUE, VTL_GREEN, VTL_RED, VTL_ORANGE};
  matrix.clear();
  matrix.setColor(col[s]);
  if ((s != ST_CONNECTING && s != ST_HOT) || blink) matrix.drawIcon(icon[s], (matrix.width() - 8) / 2);
  matrix.show();
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  dht.begin();
#if VTL_HAS_KEYPAD
  keypad.begin();
  entry.begin(2);
#endif
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html",
                "<meta name=viewport content='width=device-width'><meta http-equiv=refresh content=5>"
                "<h2>VTLabs monitor</h2><p>Status: <b>" + String(statusName[status()]) + "</b></p><p>T " +
                    String(t, 1) + " C, RH " + String(h, 0) + " %, light " +
                    String(map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100)) + " %</p><p>Limit " + String(limitC, 1) +
                    " C</p><form method=post action=/api/limit><input name=c size=4><button>Set limit</button></form>");
  });
  server.on("/api/status", HTTP_GET, []() {
    server.send(200, "application/json",
                "{\"status\":\"" + String(statusName[status()]) + "\",\"t\":" + (isnan(t) ? "null" : String(t, 1)) +
                    ",\"h\":" + (isnan(h) ? "null" : String(h, 1)) + ",\"limit\":" + String(limitC, 1) + "}");
  });
  server.on("/api/limit", HTTP_POST, []() {
    float c = server.arg("c").toFloat();
    if (c < 10 || c > 60) { server.send(400, "text/plain", "10..60"); return; }
    limitC = c;
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  static unsigned long lastDht = 0;
  static bool printedIp = false;
  if (!printedIp && WiFi.status() == WL_CONNECTED) {
    printedIp = true;
    Serial.printf("Monitor: http://%s/\n", WiFi.localIP().toString().c_str());
  }
  if (millis() - lastDht > 2500) { lastDht = millis(); t = dht.readTemperature(); h = dht.readHumidity(); }

  static Status lastStatus = ST_CONNECTING;
  Status s = status();
  if (s != lastStatus) {
    Serial.printf("status -> %s\n", statusName[s]);
    if (s == ST_HOT) tone(VTL_PIN_BUZZER, 2200, 300);
    lastStatus = s;
  }
#if VTL_HAS_KEYPAD
  char k = keypad.getKey();
  if (k == 'A') { editing = true; entry.clear(); Serial.println("limit: type 2 digits + #"); }
  else if (k && editing && entry.feed(k)) {
    long v = entry.value();
    if (v >= 10 && v <= 60) limitC = v;
    editing = false;
    Serial.printf("limit = %.0f C\n", limitC);
  }
#endif
  drawStatus();
}
