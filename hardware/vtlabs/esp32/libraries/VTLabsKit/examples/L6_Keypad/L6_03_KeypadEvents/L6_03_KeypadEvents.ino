/*
  VTLabs IoT Training Kit - L6_03 Debouncing, Hold and Release Events
  ------------------------------------------------------------------
  Purpose:     Real keys bounce. VTLKeypad only accepts a key after it was
               stable for VTL_KEYPAD_DEBOUNCE_MS, and then reports three events:
               PRESSED, HELD (after VTL_KEYPAD_HOLD_MS) and RELEASED.
  Hardware:    Matrix keypad add-on, LED_YELLOW.
  Libraries:   none
  Expected:    "PRESSED 7", "HELD 7" (if held > 0.7 s), "RELEASED 7 after 1240 ms".
               LED_YELLOW is on while a key is down.
               Holding '*' for 0.7 s prints "clear all" - a typical use of HELD.
  Experiments: 1. Change the debounce time below to 0 and to 200. What changes?
               2. Make HELD repeat every 200 ms while held (auto-repeat).
               3. Use L6_01 to watch raw scans while pressing very lightly.
  Troubleshooting:
    - Some presses ignored: you pressed shorter than the debounce time.
  @requires: keypad
*/
// Settings can be changed per sketch BEFORE the include:
#define VTL_KEYPAD_DEBOUNCE_MS 30
#define VTL_KEYPAD_HOLD_MS 700
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>

VTLKeypad keypad;
unsigned long pressedAt = 0;

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  keypad.begin();
}

void loop() {
  VTLKeyEvent e = keypad.update();
  if (!e) return;
  switch (e.type) {
    case VTL_KEY_PRESSED:
      pressedAt = millis();
      digitalWrite(VTL_PIN_LED_YELLOW, HIGH);
      Serial.printf("PRESSED %c\n", e.key);
      break;
    case VTL_KEY_HELD:
      Serial.printf("HELD %c\n", e.key);
      if (e.key == '*') Serial.println("  -> clear all");
      break;
    case VTL_KEY_RELEASED:
      digitalWrite(VTL_PIN_LED_YELLOW, LOW);
      Serial.printf("RELEASED %c after %lu ms\n", e.key, millis() - pressedAt);
      break;
    default:
      break;
  }
}
