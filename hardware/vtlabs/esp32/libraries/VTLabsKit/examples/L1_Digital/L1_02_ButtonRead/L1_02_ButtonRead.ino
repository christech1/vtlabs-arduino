/*
  VTLabs IoT Training Kit - L1_02 Read a Push Button
  ------------------------------------------------------------------
  Purpose:     Read a digital input with digitalRead() and understand
               "active LOW" buttons with pull-up resistors.
  Hardware:    BTN1 (on board, 10k pull-up to 3.3 V, pressing connects to GND).
  Libraries:   none (VTLabsKit.h only provides the pin names)
  Expected:    Serial Monitor prints 1 when idle and 0 while BTN1 is pressed,
               plus "PRESSED"/"released".
  Experiments: 1. Read BTN2 (the BOOT button) as well and print both.
               2. Remove the delay(): how fast does the output scroll?
               3. Why does an unpressed button read 1? Draw the pull-up circuit.
  Troubleshooting:
    - Always reads 0 or 1: check the pin in docs/pinmaps/, or the board revision.
    - GPIO34-39 have no internal pull-up - the board must have a resistor.
  @requires: none
*/
#include <VTLabsKit.h>

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_BTN1, VTL_PIN_BTN1_MODE);  // INPUT: the pull-up resistor is on the board
}

void loop() {
  int level = digitalRead(VTL_PIN_BTN1);
  Serial.print("BTN1 pin level = ");
  Serial.print(level);
  Serial.println(level == VTL_ACTIVE_LEVEL(BTN1) ? "  -> PRESSED" : "  -> released");
  delay(200);
}
