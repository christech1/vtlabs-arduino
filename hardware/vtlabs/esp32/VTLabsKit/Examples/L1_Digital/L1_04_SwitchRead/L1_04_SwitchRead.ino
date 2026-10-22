/*
  VTLabs IoT Training Kit - L1_04 Read a Switch (operating mode)
  ------------------------------------------------------------------
  Purpose:     A switch keeps its position - use it to select a mode.
  Hardware:    SWITCH (slide switch, on board), LED_GREEN, LED_YELLOW.
  Libraries:   none
  Expected:    Switch ON  -> "AUTO mode", green LED on.
               Switch OFF -> "MANUAL mode", yellow LED on.
               The message is printed only when the position changes.
  Experiments: 1. Count how many times the switch was changed.
               2. In AUTO mode blink the green LED instead of keeping it on.
  Troubleshooting:
    - Mode is inverted: the switch may be wired active-HIGH on your board;
      change active_low for SWITCH in the board profile (see docs).
  @requires: none
*/
#include <VTLabsKit.h>

int lastState = -1;

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_SWITCH, VTL_PIN_SWITCH_MODE);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
}

void loop() {
  int on = digitalRead(VTL_PIN_SWITCH) == VTL_ACTIVE_LEVEL(SWITCH);
  if (on != lastState) {
    lastState = on;
    Serial.println(on ? "AUTO mode" : "MANUAL mode");
    digitalWrite(VTL_PIN_LED_GREEN, on ? HIGH : LOW);
    digitalWrite(VTL_PIN_LED_YELLOW, on ? LOW : HIGH);
  }
  delay(20);
}
