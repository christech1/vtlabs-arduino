/*
  VTLabs IoT Training Kit - L2_01 Read a Potentiometer (ADC)
  ------------------------------------------------------------------
  Purpose:     Convert a voltage into a number with analogRead().
  Hardware:    POT (10k potentiometer, on board) -> VTL_PIN_POT (ADC1).
  Libraries:   none
  Expected:    Turning the knob changes the value from about 0 to 4095
               (12-bit ADC). Open Tools > Serial Plotter to see a graph.
  Experiments: 1. analogReadResolution(10): what is the new maximum?
               2. Why does the value not reach exactly 0 or 4095? (ESP32 ADC
                  is non-linear near the ends - see L2_03.)
               3. Print the value only when it changes by more than 20.
  Troubleshooting:
    - Value stuck at 0 or 4095: wiper not connected, or wrong pin.
    - Value jumps by +/-20: normal ADC noise - lesson L2_04 filters it.
  @requires: none
*/
#include <VTLabsKit.h>

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // 0..4095 (default on ESP32)
}

void loop() {
  int raw = analogRead(VTL_PIN_POT);
  Serial.print("pot:");      // "name:value" is understood by the Serial Plotter
  Serial.println(raw);
  delay(100);
}
