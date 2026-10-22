/*
  VTLabs IoT Training Kit - L5_06 Project: Complete Sensor-to-Dashboard System
  ------------------------------------------------------------------
  Purpose:     End-to-end IoT system on one board:
               sensors -> filtering -> history buffer -> JSON API -> web
               dashboard with a live chart (drawn with plain JavaScript, works
               without internet) -> alarm -> local indicators.
  Hardware:    DHT22, LDR, LED_GREEN/RED, BUZZER, OLED (optional status).
  Setup:       Wi-Fi in "arduino_secrets.h".
  Libraries:   "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    http://<IP>/ shows current values, a 60-point temperature chart
               and the alarm limit. /api/history returns the stored points.
               Above ALARM_C the red LED lights and the page turns red.
               POST /api/limit?c=30 changes the alarm limit.
  Experiments: 1. Store the history in LittleFS so it survives a reset.
               2. Add humidity as a second chart line.
               3. Add MQTT publishing from L4_06 - now it is a cloud device too.
  Troubleshooting:
    - Chart empty: wait one sample interval (5 s); check /api/history.
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include "arduino_secrets.h"

const int N = 60;  // history points
const unsigned long SAMPLE_MS = 5000;
float history[N];
int count = 0, head = 0;
float alarmC = 30.0, lastT = NAN, lastH = NAN;

WebServer server(80);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
VTLMovingAverage<4> tFilter;

const char PAGE[] PROGMEM = R"html(<!doctype html><meta name=viewport content="width=device-width">
<title>VTLabs IoT</title><style>body{font-family:sans-serif;margin:16px}.alarm{background:#fdd}</style>
<h2>VTLabs IoT Dashboard</h2><div id=v>...</div><canvas id=c width=320 height=160 style="border:1px solid #888;width:100%;max-width:640px"></canvas>
<p>Alarm limit: <input id=lim size=4> C <button onclick="fetch('/api/limit?c='+lim.value,{method:'POST'})">Set</button></p>
<script>
async function upd(){const d=await (await fetch('/api/history')).json();
document.body.className=d.alarm?'alarm':'';
v.textContent='T '+(d.t==null?'--':d.t.toFixed(1))+' C | RH '+(d.h==null?'--':d.h.toFixed(0))+' % | light '+d.light+' %';
if(document.activeElement!==lim)lim.value=d.limit;
const x=c.getContext('2d'),p=d.history;x.clearRect(0,0,320,160);if(!p.length)return;
const lo=Math.min(...p,d.limit)-1,hi=Math.max(...p,d.limit)+1,Y=t=>150-(t-lo)/(hi-lo)*140;
x.strokeStyle='red';x.beginPath();x.moveTo(0,Y(d.limit));x.lineTo(320,Y(d.limit));x.stroke();
x.strokeStyle='blue';x.beginPath();p.forEach((t,i)=>{const X=i*320/59;i?x.lineTo(X,Y(t)):x.moveTo(X,Y(t))});x.stroke();
x.fillText(hi.toFixed(1),2,10);x.fillText(lo.toFixed(1),2,158);}
setInterval(upd,3000);upd();
</script>)html";

String jsonNum(float v) { return isnan(v) ? "null" : String(v, 2); }

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.printf("\nDashboard: http://%s/\n", WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, []() { server.send_P(200, "text/html", PAGE); });
  server.on("/api/history", HTTP_GET, []() {
    String j = "{\"t\":" + jsonNum(lastT) + ",\"h\":" + jsonNum(lastH) + ",\"light\":" +
               String(map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100)) + ",\"limit\":" + String(alarmC, 1) +
               ",\"alarm\":" + (!isnan(lastT) && lastT > alarmC ? "true" : "false") + ",\"history\":[";
    for (int i = 0; i < count; i++) {
      if (i) j += ',';
      j += String(history[(head - count + i + N) % N], 2);
    }
    server.send(200, "application/json", j + "]}");
  });
  server.on("/api/limit", HTTP_POST, []() {
    float c = server.arg("c").toFloat();
    if (c >= 10 && c <= 60) { alarmC = c; server.send(204); }
    else server.send(400, "text/plain", "limit must be 10..60");
  });
  server.begin();
}

void loop() {
  server.handleClient();
  static unsigned long last = 0;
  if (millis() - last >= SAMPLE_MS) {
    last = millis();
    float t = dht.readTemperature(), h = dht.readHumidity();
    if (!isnan(t)) {
      lastT = tFilter.add(t);
      lastH = h;
      history[head] = lastT;
      head = (head + 1) % N;
      if (count < N) count++;
    }
  }
  bool alarm = !isnan(lastT) && lastT > alarmC;
  digitalWrite(VTL_PIN_LED_GREEN, !alarm);
  digitalWrite(VTL_PIN_LED_RED, alarm);
  static bool beeped = false;
  if (alarm && !beeped) { tone(VTL_PIN_BUZZER, 2500, 300); beeped = true; }
  if (!alarm) beeped = false;
}
