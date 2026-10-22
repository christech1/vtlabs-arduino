/*
  VTLabs IoT Training Kit - L4_03 Sensor Dashboard in the Browser
  ------------------------------------------------------------------
  Purpose:     Separate data (JSON API) from presentation (HTML + JavaScript).
               The page asks /api/sensors every 2 s and updates itself.
  Hardware:    DHT22, LDR, POT, PIR (on board).
  Setup:       Enter Wi-Fi details in the "arduino_secrets.h" tab.
  Libraries:   "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    http://<IP>/ shows live temperature, humidity, light, pot and
               motion. http://<IP>/api/sensors returns the raw JSON.
  Experiments: 1. Add the chip's uptime and Wi-Fi RSSI to the JSON.
               2. Colour the temperature red above 30 C (in JavaScript).
               3. Read the JSON from a Python script on your laptop.
  Troubleshooting:
    - Values show "--": the DHT read failed (see L3_05).
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include "arduino_secrets.h"

WebServer server(80);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
float temperature = NAN, humidity = NAN;

const char PAGE[] PROGMEM = R"html(<!doctype html><html><head><meta name=viewport content="width=device-width">
<title>VTLabs Dashboard</title><style>body{font-family:sans-serif;margin:16px}
.c{display:inline-block;min-width:140px;margin:6px;padding:12px;border-radius:8px;background:#eef}
.v{font-size:28px;font-weight:bold}</style></head><body><h2>VTLabs IoT Kit</h2>
<div class=c>Temperature<div class=v id=t>--</div></div><div class=c>Humidity<div class=v id=h>--</div></div>
<div class=c>Light<div class=v id=l>--</div></div><div class=c>Potentiometer<div class=v id=p>--</div></div>
<div class=c>Motion<div class=v id=m>--</div></div>
<script>
async function upd(){try{const d=await (await fetch('/api/sensors')).json();
t.textContent=d.temperature==null?'--':d.temperature.toFixed(1)+' C';
h.textContent=d.humidity==null?'--':d.humidity.toFixed(0)+' %';
l.textContent=d.light+' %';p.textContent=d.pot;m.textContent=d.motion?'YES':'no';}catch(e){}}
setInterval(upd,2000);upd();
</script></body></html>)html";

String num(float v) { return isnan(v) ? "null" : String(v, 1); }

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(VTL_PIN_PIR, VTL_PIN_PIR_MODE);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.printf("\nDashboard: http://%s/\n", WiFi.localIP().toString().c_str());

  server.on("/", []() { server.send_P(200, "text/html", PAGE); });
  server.on("/api/sensors", []() {
    String json = "{\"temperature\":" + num(temperature) + ",\"humidity\":" + num(humidity) +
                  ",\"light\":" + String(map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100)) +
                  ",\"pot\":" + String(analogRead(VTL_PIN_POT)) +
                  ",\"motion\":" + (digitalRead(VTL_PIN_PIR) ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  static unsigned long last = 0;
  if (millis() - last > 2500) {  // the DHT22 needs >= 2 s between reads
    last = millis();
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
  }
}
