/*
  VTLabs IoT Training Kit - L6_02 Read Individual Keys
  ------------------------------------------------------------------
  Purpose:     Use the VTLKeypad class: one call returns the key that was
               just pressed - scanning and debouncing are done for you.
  Hardware:    Matrix keypad add-on (Tools > Keypad (add-on)), BUZZER.
  Libraries:   none (VTLabsKeypad.h is part of the VTLabs board package)
  Expected:    Each press prints "Key: 5" once, with a short click sound.
               Digits, A-D, * and # are reported as characters.
  Experiments: 1. Light LED_RED for 'A', LED_GREEN for 'B', LED_YELLOW for 'C'.
               2. Count how many times each key was pressed.
               3. Print the key's row and column (hint: find it in VTL_KEYPAD_KEYMAP).
  Troubleshooting:
    - "Keypad not found": check I2C wiring and run L3_04 (expects 0x20).
    - Wrong characters: your keypad layout differs - change KEYPAD_KEYMAP in
      the board profile (boards/*.toml) and regenerate.
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>

VTLKeypad keypad;

void setup() {
  Serial.begin(115200);
  if (!keypad.begin()) Serial.println("Keypad not found - check wiring / Tools > Keypad");
  Serial.printf("Keypad %dx%d ready (%s)\n", VTLKeypad::ROWS, VTLKeypad::COLS, VTL_KEYPAD_OPTION_LABEL);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.printf("Key: %c\n", key);
    tone(VTL_PIN_BUZZER, 3000, 15);
  }
}
