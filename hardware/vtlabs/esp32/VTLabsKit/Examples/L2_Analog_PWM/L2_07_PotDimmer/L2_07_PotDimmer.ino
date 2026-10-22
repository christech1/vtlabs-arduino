/*
  VTLabs IoT Training Kit - L2_07 Potentiometer Dimmer
  ------------------------------------------------------------------
  Purpose:     Map an input range to an output range with map() and
               constrain(); add a dead band so the LED is fully off at the end.
  Hardware:    POT, LED_RED (on board).
  Libraries:   none
  Expected:    Knob fully left = LED off, fully right = full brightness.
  Experiments: 1. Reverse the direction (bright on the left).
               2. Use the switch to choose between two LEDs.
               3. Add a 1 s "soft start" when the LED turns on.
  Troubleshooting:
    - LED never fully off: increase DEAD_BAND.
  @requires: none
*/
#include <VTLabsKit.h>

const int DEAD_BAND = 80;  // ADC counts treated as "zero"

void setup() {
  ledcAttach(VTL_PIN_LED_RED, 5000, 8);
}

void loop() {
  int raw = analogRead(VTL_PIN_POT);
  int duty = raw < DEAD_BAND ? 0 : map(raw, DEAD_BAND, 4095, 1, 255);
  ledcWrite(VTL_PIN_LED_RED, constrain(duty, 0, 255));
  delay(20);
}
