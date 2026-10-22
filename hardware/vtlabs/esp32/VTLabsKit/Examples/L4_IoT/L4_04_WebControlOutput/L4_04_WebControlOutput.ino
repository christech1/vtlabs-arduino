/*
  VTLabs IoT Training Kit - L4_04 Control an Output from a Web Page - Safely
  ------------------------------------------------------------------
  Purpose:     Switch LEDs and the relay from a browser, with safety rules:
               only POST requests change outputs, the relay has a maximum ON
               time, and a local button always overrides the web.
  Hardware:    LED_RED, LED_GREEN, LED_YELLOW, RELAY, BTN1 (local STOP).
  SAFETY:      Low-voltage loads only on the relay.
  Setup:       Enter Wi-Fi details in the "arduino_secrets.h" tab.
  Libraries:   none
  Expected:    http://<IP>/ shows buttons for each output and their state.
               Relay switches off by itself after 60 s. BTN1 = everything OFF.
  Experiments: 1. Add a PIN code field the user must enter to switch the relay.
               2. Log who switched what (client IP: server.client().remoteIP()).
               3. Why is GET a bad choice for actions? (Browsers prefetch links.)
  Troubleshooting:
    - Nothing happens on click: check the browser console (F12).
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>
#include "arduino_secrets.h"

const unsigned long RELAY_MAX_ON_MS = 60000;
WebServer server(80);
VTLButton stopButton;
unsigned long relayOnAt = 0;
bool relayOn = false;

struct Output { const char *name; int pin; bool on; };
Output outs[] = {{"red", VTL_PIN_LED_RED, false}, {"green", VTL_PIN_LED_GREEN, false},
                 {"yellow", VTL_PIN_LED_YELLOW, false}};

void setRelay(bool on) {
  relayOn = on;
  relayOnAt = millis();
  vtlRelayWrite(on);
}

String page() {
  String s = "<!doctype html><meta name=viewport content='width=device-width'><title>VTLabs Control</title>"
             "<style>button{font-size:20px;margin:6px;padding:10px 18px}</style><h2>VTLabs outputs</h2>";
  for (auto &o : outs)
    s += "<form method=post action=/set style='display:inline'><input type=hidden name=o value=" + String(o.name) +
         "><button name=v value=" + (o.on ? "0" : "1") + ">" + o.name + ": " + (o.on ? "ON" : "off") + "</button></form>";
  s += "<form method=post action=/set><input type=hidden name=o value=relay><button name=v value=" +
       String(relayOn ? "0" : "1") + ">RELAY: " + (relayOn ? "ON" : "off") + "</button></form>";
  s += "<p>Relay switches off automatically after " + String(RELAY_MAX_ON_MS / 1000) + " s.</p>";
  return s;
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_RELAY, OUTPUT);
  setRelay(false);
  for (auto &o : outs) pinMode(o.pin, OUTPUT);
  stopButton.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.printf("\nControl page: http://%s/\n", WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, []() { server.send(200, "text/html", page()); });
  server.on("/set", HTTP_POST, []() {
    String o = server.arg("o");
    bool v = server.arg("v") == "1";
    if (o == "relay") setRelay(v);
    for (auto &out : outs)
      if (o == out.name) { out.on = v; digitalWrite(out.pin, v); }
    server.sendHeader("Location", "/");  // redirect back (Post/Redirect/Get pattern)
    server.send(303);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  stopButton.update();
  if (stopButton.pressed()) {  // local override always wins
    setRelay(false);
    for (auto &o : outs) { o.on = false; digitalWrite(o.pin, LOW); }
    Serial.println("Local STOP");
  }
  if (relayOn && millis() - relayOnAt > RELAY_MAX_ON_MS) {
    setRelay(false);
    Serial.println("Relay timeout - switched OFF");
  }
}
