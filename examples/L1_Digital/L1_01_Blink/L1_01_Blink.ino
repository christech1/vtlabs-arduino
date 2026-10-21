/*
  VTLabs IoT Training Kit - Level 1, Lesson 1: Blink

  Purpose
    Confirm the board, driver, upload and the on-board LED all work.
    Introduces pinMode(), digitalWrite() and delay().

  Board
    Tools > Board > VTLabs Training Kits > VTLabs IoT Training Kit ESP32 (Rev 1)

  Wiring
    None. Uses the on-board LED (LED_BUILTIN, defined by the VTLabs board).

  Expected behaviour
    The on-board LED turns on for 0.5 s and off for 0.5 s, forever.
    The Serial Monitor (115200 baud) prints "LED ON" / "LED OFF".

  Required libraries
    None.

  Troubleshooting
    - "LED_BUILTIN was not declared": the VTLabs board is not selected.
    - Upload stuck at "Connecting....": hold BOOT, press and release EN,
      release BOOT, then upload again. Try Upload Speed = 115200.
    - Uploads but LED does not blink: the LED may be on another GPIO on your
      board revision - report it to VTLabs with a photo of the board.
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
