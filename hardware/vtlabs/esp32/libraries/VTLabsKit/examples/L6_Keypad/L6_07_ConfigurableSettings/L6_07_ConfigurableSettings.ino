/*
  VTLabs IoT Training Kit - L6_07 Configurable System Settings
  ------------------------------------------------------------------
  Purpose:     Let the user change settings with the keypad, validate them,
               and keep them in flash (Preferences) so they survive a reset.
  Hardware:    Matrix keypad add-on, OLED, BUZZER, LED_RED.
  Libraries:   "Adafruit SSD1306" + "Adafruit GFX Library" + "Adafruit BusIO"
               (Preferences is part of the ESP32 core)
  Keys:        A/B = previous/next setting, # = edit, digits = new value,
               # = save, * = cancel/delete, hold D = factory reset.
  Settings:    Alarm temperature 10..60 C, Beep on key 0/1, LED brightness 0..100 %,
               Log interval 5..3600 s.
  Expected:    The OLED shows "Alarm temp = 30 C". After editing, the new value is
               saved; reset the board - the value is still there.
               Out-of-range values are refused with a low beep.
  Experiments: 1. Add a setting "relay max on time".
               2. Show "*" next to values that differ from the factory default.
               3. Use these settings in L9_05 (threshold) and L9_06 (alarm).
  Troubleshooting:
    - Settings reset every time: prefs.begin(..., false) must not be read-only.
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

struct Setting { const char *key, *label, *unit; long min, max, def, value; };
Setting settings[] = {
  {"alarmC", "Alarm temp", "C", 10, 60, 30, 0},
  {"beep", "Key beep", "(0/1)", 0, 1, 1, 0},
  {"bright", "LED bright.", "%", 0, 100, 50, 0},
  {"logSec", "Log interval", "s", 5, 3600, 60, 0},
};
const int N = sizeof(settings) / sizeof(settings[0]);

Adafruit_SSD1306 oled(VTL_OLED_WIDTH, VTL_OLED_HEIGHT, &Wire, -1);
VTLKeypad keypad;
VTLLineInput edit;
Preferences prefs;
int sel = 0;
bool editing = false;

void apply() { ledcWrite(VTL_PIN_LED_RED, settings[2].value * 255 / 100); }

void draw(const char *msg = "") {
  Setting &s = settings[sel];
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.printf("SETTINGS %d/%d\n\n", sel + 1, N);
  oled.setTextSize(2);
  oled.println(s.label);
  if (editing) oled.printf("%s_\n", edit.text());
  else oled.printf("%ld %s\n", s.value, s.unit);
  oled.setTextSize(1);
  oled.printf("range %ld..%ld  %s", s.min, s.max, msg);
  oled.display();
}

void setup() {
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  oled.begin(SSD1306_SWITCHCAPVCC, VTL_I2C_OLED);
  oled.setTextColor(SSD1306_WHITE);
  keypad.begin();
  ledcAttach(VTL_PIN_LED_RED, 5000, 8);
  prefs.begin("vtl-settings", false);
  for (auto &s : settings) s.value = prefs.getLong(s.key, s.def);
  apply();
  draw();
}

void loop() {
  VTLKeyEvent e = keypad.update();
  if (e.type == VTL_KEY_HELD && e.key == 'D') {
    prefs.clear();
    for (auto &s : settings) s.value = s.def;
    apply();
    tone(VTL_PIN_BUZZER, 800, 500);
    draw("RESET");
    return;
  }
  if (e.type != VTL_KEY_PRESSED) return;
  if (settings[1].value) tone(VTL_PIN_BUZZER, 3000, 10);
  Setting &s = settings[sel];

  if (!editing) {
    if (e.key == 'A') sel = (sel + N - 1) % N;
    if (e.key == 'B') sel = (sel + 1) % N;
    if (e.key == '#') { editing = true; edit.begin(5); }
    draw();
    return;
  }
  if (e.key == '*' && edit.length() == 0) { editing = false; draw("cancel"); return; }
  if (edit.feed(e.key)) {
    long v = edit.value();
    if (v < s.min || v > s.max) { tone(VTL_PIN_BUZZER, 300, 400); edit.clear(); draw("OUT OF RANGE"); return; }
    s.value = v;
    prefs.putLong(s.key, v);
    apply();
    editing = false;
    draw("saved");
    return;
  }
  draw();
}
