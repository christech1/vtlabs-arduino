/*
  VTLabs IoT Training Kit - L6_01 How a Matrix Keypad is Scanned
  ------------------------------------------------------------------
  Purpose:     A 4x4 keypad has 16 keys but only 8 wires: 4 rows x 4 columns.
               A key connects one row to one column. To find it, drive ONE row
               LOW at a time and see which column reads LOW. This sketch does
               that by hand - no keypad library.
  Hardware:    Matrix keypad add-on. Select it in Tools > Keypad (add-on).
               - PCF8574 version: keypad -> PCF8574 P0..P3 = rows, P4..P7 = columns,
                 PCF8574 on I2C at VTL_KEYPAD_I2C_ADDR (0x20).
               - Direct-GPIO version (custom boards): rows VTL_PIN_KEYPAD_R1..,
                 columns VTL_PIN_KEYPAD_C1.. (see docs/pinmaps/).
  Libraries:   none (Wire for the PCF8574 version is in the core)
  Expected:    Serial Monitor prints a live 4x4 grid; pressed keys show as #.
               Below it: "row 2, col 3 -> key 6".
  Experiments: 1. Press two keys in the same row, then in the same column.
               2. Press three keys forming an L shape: a "ghost" key may appear.
                  Why? (Look up "keypad ghosting" and diodes.)
               3. Slow the scan (delay 500 ms) - can you miss a quick press?
  Troubleshooting:
    - Grid never changes (PCF8574): run L3_04, check address and I2C wiring.
    - Wrong keys: rows/columns swapped or keypad connector reversed.
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>  // only for the pin/size settings and keymap; scanning is done below

#if VTL_KEYPAD_BACKEND == VTL_KEYPAD_BACKEND_PCF8574
#include <Wire.h>
// PCF8574: writing 1 to a pin makes it a weak pull-up input, 0 pulls it LOW.
uint8_t readRow(uint8_t row) {
  Wire.beginTransmission(VTL_KEYPAD_I2C_ADDR);
  Wire.write((uint8_t) ~(1 << row));  // only this row LOW
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)VTL_KEYPAD_I2C_ADDR, (uint8_t)1);
  uint8_t v = Wire.available() ? Wire.read() : 0xFF;
  return (~v >> 4) & 0x0F;  // columns P4..P7, 1 = pressed
}
void setupKeypad() { Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL); }
#else
const uint8_t rows[VTL_KEYPAD_ROWS] = {VTL_PIN_KEYPAD_R1, VTL_PIN_KEYPAD_R2, VTL_PIN_KEYPAD_R3, VTL_PIN_KEYPAD_R4};
const uint8_t cols[VTL_KEYPAD_COLS] = {VTL_PIN_KEYPAD_C1, VTL_PIN_KEYPAD_C2, VTL_PIN_KEYPAD_C3
#if VTL_KEYPAD_COLS > 3
                                       , VTL_PIN_KEYPAD_C4
#endif
};
uint8_t readRow(uint8_t row) {
  pinMode(rows[row], OUTPUT);
  digitalWrite(rows[row], LOW);
  delayMicroseconds(10);
  uint8_t bits = 0;
  for (uint8_t c = 0; c < VTL_KEYPAD_COLS; c++)
    if (digitalRead(cols[c]) == LOW) bits |= 1 << c;
  pinMode(rows[row], INPUT);  // release the row (high impedance)
  return bits;
}
void setupKeypad() {
  for (uint8_t r = 0; r < VTL_KEYPAD_ROWS; r++) pinMode(rows[r], INPUT);
  for (uint8_t c = 0; c < VTL_KEYPAD_COLS; c++) pinMode(cols[c], VTL_KEYPAD_COL_INPUT_MODE);
}
#endif

void setup() {
  Serial.begin(115200);
  setupKeypad();
}

void loop() {
  String grid = "\n   ";
  String hits;
  for (uint8_t c = 0; c < VTL_KEYPAD_COLS; c++) grid += " C" + String(c + 1);
  for (uint8_t r = 0; r < VTL_KEYPAD_ROWS; r++) {
    uint8_t bits = readRow(r);
    grid += "\nR" + String(r + 1) + " ";
    for (uint8_t c = 0; c < VTL_KEYPAD_COLS; c++) {
      bool down = bits & (1 << c);
      grid += down ? "  #" : "  .";
      if (down) hits += "row " + String(r + 1) + ", col " + String(c + 1) + " -> key " +
                        VTL_KEYPAD_KEYMAP[r * VTL_KEYPAD_COLS + c] + "\n";
    }
  }
  Serial.println(grid);
  Serial.print(hits.length() ? hits : "no key\n");
  delay(300);
}
