/*
  VTLabs IoT Training Kit - L5_05 Project: Local Wi-Fi Control (no router)
  ------------------------------------------------------------------
  Purpose:     The kit creates its OWN Wi-Fi network (access point). A phone
               joins it and controls the kit - works in a lab without internet.
  Hardware:    LEDs, RELAY, BUZZER, POT, LDR (on board).
  SAFETY:      Low-voltage relay loads only.
  Libraries:   none
  Expected:    Network "VTLabs-XXXX" (password vtlabs123) appears.
               Connect the phone, open http://192.168.4.1/
               Sliders set LED brightness, buttons switch the relay and beep,
               live readings refresh every second.
  Experiments: 1. Change the network name to include your group name.
               2. Limit to 1 connected phone (WiFi.softAP(...,..., channel, hidden, 1)).
               3. Add a "scene" button: all LEDs to 50 %.
  Troubleshooting:
    - Phone disconnects ("no internet"): tell the phone to stay connected.
    - Password must be at least 8 characters, or the AP will not start.
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>

const char *AP_PASS = "vtlabs123";
WebServer server(80);
const int ledPins[3] = {VTL_PIN_LED_RED, VTL_PIN_LED_GREEN, VTL_PIN_LED_YELLOW};
int ledValue[3] = {0, 0, 0};
bool relayOn = false;

const char PAGE[] PROGMEM = R"html(<!doctype html><meta name=viewport content="width=device-width">
<title>VTLabs Local Control</title><style>body{font-family:sans-serif;margin:16px}input{width:90%}
button{font-size:18px;padding:10px;margin:4px}</style><h2>VTLabs Local Control</h2>
Red<br><input type=range min=0 max=255 value=0 oninput="s(0,this.value)"><br>
Green<br><input type=range min=0 max=255 value=0 oninput="s(1,this.value)"><br>
Yellow<br><input type=range min=0 max=255 value=0 oninput="s(2,this.value)"><br>
<button onclick="fetch('/relay',{method:'POST'})">Relay ON/OFF</button>
<button onclick="fetch('/beep',{method:'POST'})">Beep</button>
<p id=r>...</p><script>
function s(i,v){fetch('/led?i='+i+'&v='+v,{method:'POST'})}
setInterval(async()=>{const d=await (await fetch('/state')).json();
r.textContent='Pot '+d.pot+' | Light '+d.light+' | Relay '+(d.relay?'ON':'off')},1000);
</script>)html";

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) ledcAttach(ledPins[i], 5000, 8);
  pinMode(VTL_PIN_RELAY, OUTPUT);
  vtlRelayWrite(false);

  String ssid = "VTLabs-" + String((uint32_t)ESP.getEfuseMac() & 0xFFFF, HEX);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), AP_PASS);
  Serial.printf("Join \"%s\" (password %s), open http://%s/\n", ssid.c_str(), AP_PASS,
                WiFi.softAPIP().toString().c_str());

  server.on("/", HTTP_GET, []() { server.send_P(200, "text/html", PAGE); });
  server.on("/led", HTTP_POST, []() {
    int i = server.arg("i").toInt(), v = constrain(server.arg("v").toInt(), 0, 255);
    if (i >= 0 && i < 3) { ledValue[i] = v; ledcWrite(ledPins[i], v); }
    server.send(204);
  });
  server.on("/relay", HTTP_POST, []() { relayOn = !relayOn; vtlRelayWrite(relayOn); server.send(204); });
  server.on("/beep", HTTP_POST, []() { tone(VTL_PIN_BUZZER, 2000, 150); server.send(204); });
  server.on("/state", HTTP_GET, []() {
    server.send(200, "application/json",
                "{\"pot\":" + String(analogRead(VTL_PIN_POT)) + ",\"light\":" + String(analogRead(VTL_PIN_LDR)) +
                    ",\"relay\":" + (relayOn ? "true" : "false") + "}");
  });
  server.begin();
}

void loop() {
  server.handleClient();
}
