/*
  VTLabs IoT Training Kit - L9_01 Exercise 1: Keypad-Controlled LEDs
  ------------------------------------------------------------------
  Purpose:     Select an LED and set its brightness from the keypad.
  Hardware:    Matrix keypad add-on, LED_RED/GREEN/YELLOW, BUZZER.
               OPTIONAL: LED matrix shows the selected LED's level as a bar.
  Libraries:   none (with a matrix: see L8_01)
  Keys:        1/2/3 select red/green/yellow     A = brighter   B = dimmer
               C = blink on/off for selected LED   0 = selected off   # = all on
               D = all off
  Expected:    Serial: "red: 75 %". Each step is 25 %. The selected LED blinks
               once when selected so you can see which one it is.
  Task:        Extend it: '*' then two digits sets an exact level, e.g. * 4 0 = 40 %.
  @requires: keypad
  @optional: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#if VTL_HAS_MATRIX
#include <VTLabsMatrix.h>
VTLMatrix matrix;
#endif

VTLKeypad keypad;
const int pins[3] = {VTL_PIN_LED_RED, VTL_PIN_LED_GREEN, VTL_PIN_LED_YELLOW};
const char *names[3] = {"red", "green", "yellow"};
int level[3] = {0, 0, 0};  // percent
bool blinkMode[3] = {false, false, false};
int sel = 0;

void apply() {
  for (int i = 0; i < 3; i++) {
    bool on = !blinkMode[i] || (millis() / 300) % 2;
    ledcWrite(pins[i], on ? level[i] * 255 / 100 : 0);
  }
}

void report() {
  Serial.printf("%s: %d %%%s\n", names[sel], level[sel], blinkMode[sel] ? " (blink)" : "");
#if VTL_HAS_MATRIX
  matrix.clear();
  const uint32_t c[3] = {VTL_RED, VTL_GREEN, VTL_YELLOW};
  matrix.setColor(c[sel]);
  matrix.drawLevel(level[sel]);
  matrix.show();
#endif
}

void setup() {
  Serial.begin(115200);
  for (int p : pins) ledcAttach(p, 5000, 8);
  keypad.begin();
#if VTL_HAS_MATRIX
  matrix.begin();
#endif
  report();
}

void loop() {
  apply();
  char k = keypad.getKey();
  if (!k) return;
  tone(VTL_PIN_BUZZER, 3000, 10);
  if (k >= '1' && k <= '3') {
    sel = k - '1';
    ledcWrite(pins[sel], 255); delay(120); ledcWrite(pins[sel], 0); delay(120);  // identify
  }
  if (k == 'A') level[sel] = min(100, level[sel] + 25);
  if (k == 'B') level[sel] = max(0, level[sel] - 25);
  if (k == 'C') blinkMode[sel] = !blinkMode[sel];
  if (k == '0') level[sel] = 0;
  if (k == '#') for (int &l : level) l = 100;
  if (k == 'D') for (int &l : level) l = 0;
  report();
}
