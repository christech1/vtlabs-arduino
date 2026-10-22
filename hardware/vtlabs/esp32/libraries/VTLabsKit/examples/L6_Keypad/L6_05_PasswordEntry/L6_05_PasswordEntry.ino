/*
  VTLabs IoT Training Kit - L6_05 Password (PIN) Entry
  ------------------------------------------------------------------
  Purpose:     Enter a hidden PIN, compare it, limit wrong attempts with a
               lock-out timer, and change the PIN (stored in flash).
               Keys: digits, * = delete, # = enter.
               Type "A" then old PIN # then new PIN # to change the PIN.
  Hardware:    Matrix keypad add-on, LED_GREEN, LED_RED, BUZZER.
  Libraries:   none (Preferences is part of the ESP32 core)
  Expected:    Serial shows "PIN: ****". Correct PIN -> "ACCESS GRANTED",
               green LED 3 s. Wrong -> red LED; after 3 wrong attempts the
               keypad is locked for 30 s. Default PIN: 1234.
  Experiments: 1. Increase the lock-out time after every failed lock-out.
               2. Why is comparing PINs with == a (small) security risk?
                  Look up "timing attack".
               3. Add a second user with a different PIN.
  Troubleshooting:
    - Forgot the PIN: hold BTN1 during reset to restore 1234.
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <Preferences.h>

const uint8_t MAX_TRIES = 3;
const unsigned long LOCK_MS = 30000;

VTLKeypad keypad;
VTLLineInput pin;
Preferences prefs;
String storedPin;
uint8_t wrong = 0;
unsigned long lockedUntil = 0;
enum Mode { ENTER, CHANGE_OLD, CHANGE_NEW } mode = ENTER;

void feedback(bool ok) {
  digitalWrite(ok ? VTL_PIN_LED_GREEN : VTL_PIN_LED_RED, HIGH);
  tone(VTL_PIN_BUZZER, ok ? 2500 : 300, ok ? 150 : 600);
  delay(ok ? 3000 : 800);
  digitalWrite(VTL_PIN_LED_GREEN, LOW);
  digitalWrite(VTL_PIN_LED_RED, LOW);
}

void prompt() {
  Serial.println(mode == ENTER ? "Enter PIN, then #" : mode == CHANGE_OLD ? "Old PIN, then #" : "New PIN (4-6 digits), then #");
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  pinMode(VTL_PIN_BTN1, VTL_PIN_BTN1_MODE);
  keypad.begin();
  pin.begin(6, true);  // letters allowed so 'A' can start "change PIN"
  prefs.begin("vtl-pin", false);
  if (digitalRead(VTL_PIN_BTN1) == VTL_ACTIVE_LEVEL(BTN1)) prefs.putString("pin", "1234");
  storedPin = prefs.getString("pin", "1234");
  prompt();
}

void loop() {
  if (millis() < lockedUntil) return;  // locked: ignore keys
  char key = keypad.getKey();
  if (!key) return;
  if (mode == ENTER && key == 'A' && pin.length() == 0) { mode = CHANGE_OLD; prompt(); return; }

  if (!pin.feed(key)) {
    Serial.println("PIN: " + pin.masked());
    tone(VTL_PIN_BUZZER, 3000, 10);
    return;
  }
  String entered = pin.text();
  pin.clear();

  if (mode == CHANGE_NEW) {
    bool valid = entered.length() >= 4 && entered.indexOf('A') < 0 && entered.indexOf('B') < 0 &&
                 entered.indexOf('C') < 0 && entered.indexOf('D') < 0;
    if (valid) { prefs.putString("pin", entered); storedPin = entered; Serial.println("PIN changed"); }
    else Serial.println("New PIN must be 4-6 digits");
    feedback(valid);
    mode = ENTER;
  } else if (entered == storedPin) {
    wrong = 0;
    if (mode == CHANGE_OLD) mode = CHANGE_NEW;
    else { Serial.println("ACCESS GRANTED"); feedback(true); }
  } else {
    wrong++;
    mode = ENTER;
    Serial.printf("WRONG PIN (%d/%d)\n", wrong, MAX_TRIES);
    feedback(false);
    if (wrong >= MAX_TRIES) {
      wrong = 0;
      lockedUntil = millis() + LOCK_MS;
      Serial.printf("LOCKED for %lu s\n", LOCK_MS / 1000);
    }
  }
  prompt();
}
