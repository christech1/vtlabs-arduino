/*
  VTLabs IoT Training Kit - L6_08 Keypad as the Kit's Control Panel
  ------------------------------------------------------------------
  Purpose:     Combine keypad input with LEDs, sensors, display and actuators.
  Hardware:    Matrix keypad add-on, LEDs, RELAY, BUZZER, DHT22, LDR, OLED.
  SAFETY:      Low-voltage relay loads only. The relay switches off after 30 s.
  Libraries:   "Adafruit SSD1306" + "Adafruit GFX Library" + "Adafruit BusIO",
               "DHT sensor library" + "Adafruit Unified Sensor"
  Key map:     1/2/3 toggle red/green/yellow LED   4 relay on/off
               5 beep                              7 show temperature
               8 show light                        0 everything off
               A..D: brightness 25/50/75/100 % for all LEDs
  Expected:    Each key acts immediately; the OLED shows the last action and
               the state of all outputs.
  Experiments: 1. Make '9' run the traffic light from L1_08.
               2. Require '#' after '4' to confirm switching the relay.
  Troubleshooting:
    - LEDs do not dim: they must be attached with ledcAttach() (done in setup).
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

Adafruit_SSD1306 oled(VTL_OLED_WIDTH, VTL_OLED_HEIGHT, &Wire, -1);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
VTLKeypad keypad;

const int leds[3] = {VTL_PIN_LED_RED, VTL_PIN_LED_GREEN, VTL_PIN_LED_YELLOW};
bool ledOn[3] = {false, false, false};
int brightness = 255;
bool relayOn = false;
unsigned long relayAt = 0;
String lastAction = "ready";

void refreshOutputs() {
  for (int i = 0; i < 3; i++) ledcWrite(leds[i], ledOn[i] ? brightness : 0);
  vtlRelayWrite(relayOn);
}

void draw() {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("KEYPAD CONTROL");
  oled.printf("R:%s G:%s Y:%s\n", ledOn[0] ? "on" : "--", ledOn[1] ? "on" : "--", ledOn[2] ? "on" : "--");
  oled.printf("Relay:%s  Bright:%d%%\n\n", relayOn ? "ON" : "off", brightness * 100 / 255);
  oled.setTextSize(2);
  oled.println(lastAction);
  oled.setTextSize(1);
  oled.display();
}

void setup() {
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  oled.begin(SSD1306_SWITCHCAPVCC, VTL_I2C_OLED);
  oled.setTextColor(SSD1306_WHITE);
  dht.begin();
  keypad.begin();
  for (int p : leds) ledcAttach(p, 5000, 8);
  pinMode(VTL_PIN_RELAY, OUTPUT);
  refreshOutputs();
  draw();
}

void loop() {
  char k = keypad.getKey();
  if (relayOn && millis() - relayAt > 30000) { relayOn = false; lastAction = "relay tmo"; refreshOutputs(); draw(); }
  if (!k) return;
  switch (k) {
    case '1': case '2': case '3': { int i = k - '1'; ledOn[i] = !ledOn[i]; lastAction = String("LED ") + k; break; }
    case '4': relayOn = !relayOn; relayAt = millis(); lastAction = relayOn ? "relay ON" : "relay off"; break;
    case '5': tone(VTL_PIN_BUZZER, 2000, 150); lastAction = "beep"; break;
    case '7': { float t = dht.readTemperature(); lastAction = isnan(t) ? "T: --" : String(t, 1) + " C"; break; }
    case '8': lastAction = "L: " + String(map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100)) + "%"; break;
    case '0': for (bool &b : ledOn) b = false; relayOn = false; lastAction = "ALL OFF"; break;
    case 'A': case 'B': case 'C': case 'D': brightness = (k - 'A' + 1) * 255 / 4; lastAction = "bright"; break;
    default: lastAction = String("key ") + k; break;
  }
  refreshOutputs();
  draw();
}
