/*
  VTLabs IoT Training Kit - L9_03 Exercise 3: Access-Control Demonstration
  ------------------------------------------------------------------
  Purpose:     A door-lock simulation: PIN entry, status indicators, an
               actuator with automatic re-lock, and an event log.
  Hardware:    Matrix keypad add-on, RELAY (= door lock, low-voltage demo only),
               LED_GREEN (unlocked), LED_RED (locked/denied), BUZZER, BTN1 (inside
               "exit" button). OPTIONAL: LED matrix shows lock/unlock/cross icons.
  Libraries:   none (with a matrix: see L8_01)
  Users:       PIN 1234 = "Teacher", 2580 = "Student". Change them below.
  Expected:    Locked: red LED, lock icon. Correct PIN + # -> relay ON for 5 s,
               green LED, unlock icon, "OPEN by Teacher". Wrong PIN -> cross icon,
               long beep. 3 wrong PINs -> 60 s lock-out with blinking red LED.
               BTN1 opens the door from inside without a PIN.
               Every event is printed with a time stamp (the "audit log").
  Task:        Store the last 20 events in Preferences and print them on 'D'.
  @requires: keypad
  @optional: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#if VTL_HAS_MATRIX
#include <VTLabsMatrix.h>
VTLMatrix matrix;
#endif

struct User { const char *pin, *name; };
const User users[] = {{"1234", "Teacher"}, {"2580", "Student"}};
const unsigned long OPEN_MS = 5000, LOCKOUT_MS = 60000;

VTLKeypad keypad;
VTLLineInput pin;
VTLButton exitBtn;
bool open_ = false;
unsigned long openedAt = 0, lockoutUntil = 0;
int failures = 0;

#if VTL_HAS_MATRIX
void showIcon(const uint8_t *icon, uint32_t colour) {
  matrix.clear();
  matrix.setColor(colour);
  matrix.drawIcon(icon, (matrix.width() - 8) / 2);
  matrix.show();
}
#endif

void logEvent(const String &what) { Serial.printf("[%7lu s] %s\n", millis() / 1000, what.c_str()); }

void setLock(bool openDoor, const String &who) {
  open_ = openDoor;
  openedAt = millis();
  vtlRelayWrite(openDoor);
  digitalWrite(VTL_PIN_LED_GREEN, openDoor);
  digitalWrite(VTL_PIN_LED_RED, !openDoor);
#if VTL_HAS_MATRIX
  showIcon(openDoor ? vtl_icons::UNLOCK : vtl_icons::LOCK, openDoor ? VTL_GREEN : VTL_RED);
#endif
  logEvent(openDoor ? "OPEN by " + who : "LOCKED");
  if (openDoor) tone(VTL_PIN_BUZZER, 2500, 150);
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_RELAY, OUTPUT);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  exitBtn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  keypad.begin();
  pin.begin(6);
#if VTL_HAS_MATRIX
  matrix.begin();
#endif
  setLock(false, "");
}

void loop() {
  exitBtn.update();
  if (exitBtn.pressed() && !open_) setLock(true, "EXIT button");
  if (open_ && millis() - openedAt > OPEN_MS) setLock(false, "");

  if (millis() < lockoutUntil) {
    digitalWrite(VTL_PIN_LED_RED, (millis() / 200) % 2);
    keypad.getKey();  // read and ignore keys
    return;
  }
  char k = keypad.getKey();
  if (!k || open_) return;
  tone(VTL_PIN_BUZZER, 3000, 10);
  if (!pin.feed(k)) return;

  for (const User &u : users) {
    if (strcmp(pin.text(), u.pin) == 0) {
      pin.clear();
      failures = 0;
      setLock(true, u.name);
      return;
    }
  }
  pin.clear();
  failures++;
  logEvent("DENIED (attempt " + String(failures) + ")");
#if VTL_HAS_MATRIX
  showIcon(vtl_icons::CROSS, VTL_RED);
#endif
  tone(VTL_PIN_BUZZER, 300, 700);
  if (failures >= 3) {
    failures = 0;
    lockoutUntil = millis() + LOCKOUT_MS;
    logEvent("LOCK-OUT 60 s");
  }
}
