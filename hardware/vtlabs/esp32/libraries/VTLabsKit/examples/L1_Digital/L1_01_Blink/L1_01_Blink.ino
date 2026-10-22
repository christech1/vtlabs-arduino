/*
  VTLabs IoT Training Kit - L1_01 Blink
  ------------------------------------------------------------------
  Purpose:     Check that board, driver and upload work. Learn pinMode(),
               digitalWrite() and delay().
  Hardware:    On-board LED (LED_BUILTIN) - no wiring.
  Libraries:   none
  Expected:    LED on 0.5 s, off 0.5 s. Serial Monitor (115200): "LED ON" / "LED OFF".
  Experiments: 1. Change both delays to 100 ms. What do you see?
               2. Make a "heartbeat": two short blinks, then a pause.
               3. Blink VTL_PIN_LED_RED instead (add #include <VTLabsKit.h>).
  Troubleshooting:
    - "LED_BUILTIN was not declared": select the VTLabs board in Tools > Board.
    - Upload stuck at "Connecting...": hold BOOT, tap EN, release BOOT, upload again.
    - Uploads but no blink: your board revision may use another LED pin - report it.
  @requires: none
*/

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON");
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED OFF");
  delay(500);
}
