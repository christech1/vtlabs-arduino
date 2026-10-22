/*
  VTLabs IoT Training Kit - L6_04 Numeric Input with Validation
  ------------------------------------------------------------------
  Purpose:     Type a number on the keypad, edit it, confirm it, and check
               that it is within the allowed range.
               Keys: 0-9 = digit, * = delete last digit, hold * = clear,
                     # = enter.
  Hardware:    Matrix keypad add-on, BUZZER, LED_GREEN, LED_RED.
  Libraries:   none
  Expected:    Serial shows "Input: 12_" while typing.
               '#' with a value 1..1000 -> "Accepted: 125" + green LED + high beep.
               Out of range -> "Rejected" + red LED + low beep.
  Experiments: 1. Use 'A' as a decimal point and accept 12.5 (allowLetters...).
               2. Change the range to a voltage 0..3300 mV and set LED_RED's
                  brightness with it (ledcWrite).
               3. Add a 10 s timeout that cancels a half-typed number.
  Troubleshooting:
    - Number does not grow beyond 4 digits: MAX_DIGITS limits it.
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>

const uint8_t MAX_DIGITS = 4;
const long MIN_VALUE = 1, MAX_VALUE = 1000;

VTLKeypad keypad;
VTLLineInput input;

void showInput() { Serial.printf("Input: %s_\n", input.text()); }

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  keypad.begin();
  input.begin(MAX_DIGITS);
  Serial.printf("Enter a number %ld..%ld, then #\n", MIN_VALUE, MAX_VALUE);
}

void loop() {
  VTLKeyEvent e = keypad.update();
  if (e.type == VTL_KEY_HELD && e.key == '*') { input.clear(); showInput(); return; }
  if (e.type != VTL_KEY_PRESSED) return;

  digitalWrite(VTL_PIN_LED_GREEN, LOW);
  digitalWrite(VTL_PIN_LED_RED, LOW);
  if (input.feed(e.key)) {  // '#' pressed with something typed
    long v = input.value();
    bool ok = v >= MIN_VALUE && v <= MAX_VALUE;
    Serial.printf("%s: %ld\n", ok ? "Accepted" : "Rejected (out of range)", v);
    digitalWrite(ok ? VTL_PIN_LED_GREEN : VTL_PIN_LED_RED, HIGH);
    tone(VTL_PIN_BUZZER, ok ? 2500 : 400, ok ? 120 : 400);
    input.clear();
  } else {
    tone(VTL_PIN_BUZZER, 3000, 10);
    showInput();
  }
}
