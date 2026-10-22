/*
  VTLabs IoT Training Kit - L9_05 Exercise 5: Threshold Set Through the Keypad
  ------------------------------------------------------------------
  Purpose:     A light-controlled output whose switching threshold is entered
               on the keypad, validated, saved in flash, and used with hysteresis.
  Hardware:    Matrix keypad add-on, LDR, LED_YELLOW (the controlled "lamp"),
               RELAY (optional load). OPTIONAL: LED matrix shows level vs threshold.
  Libraries:   none (with a matrix: see L8_01)
  Keys:        A = enter new threshold (0..100 % light), then digits and #
               B = show current threshold     * = cancel/delete
  Expected:    Serial every second: "light 42 %  threshold 30 %  lamp OFF".
               When light < threshold - 3 %: lamp ON; above threshold + 3 %: OFF.
               The threshold survives a reset (Preferences).
               Matrix: bar = current light; one blinking column = threshold.
  Task:        Add a second threshold for the relay (two-stage control).
  @requires: keypad
  @optional: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <Preferences.h>
#if VTL_HAS_MATRIX
#include <VTLabsMatrix.h>
VTLMatrix matrix;
#endif

const int HYST = 3;  // percent
VTLKeypad keypad;
VTLLineInput input;
Preferences prefs;
int threshold;
bool editing = false, lampOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  pinMode(VTL_PIN_RELAY, OUTPUT);
  vtlRelayWrite(false);
  keypad.begin();
  prefs.begin("vtl-thresh", false);
  threshold = prefs.getInt("light", 30);
#if VTL_HAS_MATRIX
  matrix.begin();
#endif
  Serial.printf("Threshold %d %% (A = change)\n", threshold);
}

void loop() {
  int light = map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100);
  if (!lampOn && light < threshold - HYST) lampOn = true;
  if (lampOn && light > threshold + HYST) lampOn = false;
  digitalWrite(VTL_PIN_LED_YELLOW, lampOn);
  vtlRelayWrite(lampOn);

  char k = keypad.getKey();
  if (k) {
    tone(VTL_PIN_BUZZER, 3000, 10);
    if (!editing && k == 'A') { editing = true; input.begin(3); Serial.println("New threshold (0-100), then #"); }
    else if (!editing && k == 'B') Serial.printf("Threshold is %d %%\n", threshold);
    else if (editing && k == '*' && input.length() == 0) { editing = false; Serial.println("cancelled"); }
    else if (editing && input.feed(k)) {
      long v = input.value();
      editing = false;
      if (v <= 100) { threshold = v; prefs.putInt("light", threshold); Serial.printf("Saved %d %%\n", threshold); }
      else { tone(VTL_PIN_BUZZER, 300, 400); Serial.println("Must be 0..100"); }
    } else if (editing) Serial.printf("  %s_\n", input.text());
  }

  static unsigned long last = 0;
  if (millis() - last > 1000) {
    last = millis();
    Serial.printf("light %d %%  threshold %d %%  lamp %s\n", light, threshold, lampOn ? "ON" : "OFF");
  }
#if VTL_HAS_MATRIX
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 100) {
    lastDraw = millis();
    matrix.clear();
    matrix.setColor(lampOn ? VTL_YELLOW : VTL_BLUE);
    matrix.drawLevel(light);
    int tx = constrain(threshold * matrix.width() / 100, 0, matrix.width() - 1);
    matrix.setColor(VTL_RED);
    if ((millis() / 300) % 2)
      for (int y = 0; y < matrix.height(); y++) matrix.setPixel(tx, y);
    matrix.show();
  }
#endif
}
