/*
  VTLabs IoT Training Kit - L9_06 Exercise 6: Alarm System with Keypad Acknowledgement
  ------------------------------------------------------------------
  Purpose:     A small alarm panel built as a state machine:
                 DISARMED -> (PIN + A) -> EXIT DELAY 10 s -> ARMED
                 ARMED -> (motion, or temperature > 40 C) -> ALARM
                 ALARM -> (PIN + #) -> ACKNOWLEDGED (silenced, red LED stays on)
                 ACKNOWLEDGED -> (PIN + D) -> DISARMED
  Hardware:    Matrix keypad add-on, PIR, DHT22, BUZZER, LED_GREEN/YELLOW/RED.
               OPTIONAL: LED matrix shows the state icon.
  Libraries:   "DHT sensor library" + "Adafruit Unified Sensor" (with a matrix: see L8_01)
  Keys:        type PIN (default 1234), then  A = arm,  # = acknowledge,  D = disarm
  Expected:    Green = disarmed, yellow blinking = exit delay, yellow = armed,
               red blinking + siren = ALARM, red steady = acknowledged.
               Every state change is printed with the reason.
  Task:        Add an entry delay: motion gives 10 s to type the PIN before the siren.
  @requires: keypad
  @optional: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <DHT.h>
#if VTL_HAS_MATRIX
#include <VTLabsMatrix.h>
VTLMatrix matrix;
#endif

const char *PIN = "1234";
const float TEMP_LIMIT = 40.0;
const unsigned long EXIT_DELAY_MS = 10000;

enum State { DISARMED, EXIT_DELAY, ARMED, ALARM, ACKED };
const char *stateName[] = {"DISARMED", "EXIT DELAY", "ARMED", "ALARM", "ACKNOWLEDGED"};
State state = DISARMED;
unsigned long stateSince = 0;

VTLKeypad keypad;
VTLLineInput entry;
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);

void go(State s, const char *why) {
  state = s;
  stateSince = millis();
  Serial.printf("[%lu s] %s  (%s)\n", millis() / 1000, stateName[s], why);
  if (s != ALARM) noTone(VTL_PIN_BUZZER);
}

void indicators() {
  bool blink = (millis() / 250) % 2;
  digitalWrite(VTL_PIN_LED_GREEN, state == DISARMED);
  digitalWrite(VTL_PIN_LED_YELLOW, state == ARMED || (state == EXIT_DELAY && blink));
  digitalWrite(VTL_PIN_LED_RED, state == ACKED || (state == ALARM && blink));
  if (state == ALARM) tone(VTL_PIN_BUZZER, blink ? 1800 : 1200);  // two-tone siren
#if VTL_HAS_MATRIX
  static unsigned long last = 0;
  if (millis() - last < 100) return;
  last = millis();
  const uint8_t *icon[] = {vtl_icons::UNLOCK, vtl_icons::ARROW_UP, vtl_icons::LOCK, vtl_icons::WARNING,
                           vtl_icons::BELL};
  const uint32_t col[] = {VTL_GREEN, VTL_YELLOW, VTL_YELLOW, VTL_RED, VTL_ORANGE};
  matrix.clear();
  matrix.setColor(col[state]);
  if (state != ALARM || blink) matrix.drawIcon(icon[state], (matrix.width() - 8) / 2);
  matrix.show();
#endif
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_PIR, VTL_PIN_PIR_MODE);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  dht.begin();
  keypad.begin();
  entry.begin(6, true);
#if VTL_HAS_MATRIX
  matrix.begin();
#endif
  go(DISARMED, "power on");
}

void loop() {
  indicators();
  // sensors
  static unsigned long lastDht = 0;
  static float t = NAN;
  if (millis() - lastDht > 2500) { lastDht = millis(); t = dht.readTemperature(); }
  if (state == EXIT_DELAY && millis() - stateSince > EXIT_DELAY_MS) go(ARMED, "exit delay over");
  if (state == ARMED && digitalRead(VTL_PIN_PIR)) go(ALARM, "motion");
  if (state == ARMED && !isnan(t) && t > TEMP_LIMIT) go(ALARM, "over-temperature");

  // keypad: PIN followed by a command letter or '#'
  char k = keypad.getKey();
  if (!k) return;
  if (k == 'A' || k == 'D' || k == '#') {
    bool ok = strcmp(entry.text(), PIN) == 0;
    entry.clear();
    if (!ok) { Serial.println("wrong PIN"); tone(VTL_PIN_BUZZER, 300, 300); return; }
    if (k == 'A' && state == DISARMED) go(EXIT_DELAY, "armed by user");
    else if (k == '#' && state == ALARM) go(ACKED, "acknowledged by user");
    else if (k == 'D' && state != ALARM) go(DISARMED, "disarmed by user");
    else Serial.println("command not allowed in this state");
    return;
  }
  entry.feed(k);
  if (state != ALARM) tone(VTL_PIN_BUZZER, 3000, 10);
}
