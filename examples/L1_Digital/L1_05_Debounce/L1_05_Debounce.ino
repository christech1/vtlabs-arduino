/*
  VTLabs IoT Training Kit - L1_05 Debouncing
  ------------------------------------------------------------------
  Purpose:     Mechanical contacts bounce for a few milliseconds. Count raw
               edges and debounced presses side by side, using millis()
               instead of delay().
  Hardware:    BTN1 (on board).
  Libraries:   none (the VTLButton class in VTLabsKit does the same thing)
  Expected:    Each press prints "raw edges: N   debounced presses: M".
               N is often bigger than M - those extra edges are bounce.
  Experiments: 1. Set DEBOUNCE_MS to 0, 5, 50, 300. Which value feels right?
               2. Replace the code with VTLButton from <VTLabsKit.h>.
  Troubleshooting:
    - raw == debounced always: your button bounces little; try BTN2 or tap fast.
  @requires: none
*/
#include <VTLabsKit.h>

const unsigned long DEBOUNCE_MS = 25;

int lastRaw;
int stableState;
unsigned long lastChange = 0;
unsigned long rawEdges = 0, presses = 0;

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_BTN1, VTL_PIN_BTN1_MODE);
  lastRaw = stableState = digitalRead(VTL_PIN_BTN1);
}

void loop() {
  int raw = digitalRead(VTL_PIN_BTN1);
  if (raw != lastRaw) {  // something changed: maybe bounce
    lastRaw = raw;
    lastChange = millis();
    rawEdges++;
  }
  // accept the new level only after it was stable for DEBOUNCE_MS
  if (raw != stableState && millis() - lastChange >= DEBOUNCE_MS) {
    stableState = raw;
    if (stableState == VTL_ACTIVE_LEVEL(BTN1)) {
      presses++;
      Serial.printf("raw edges: %lu   debounced presses: %lu\n", rawEdges, presses);
    }
  }
}
