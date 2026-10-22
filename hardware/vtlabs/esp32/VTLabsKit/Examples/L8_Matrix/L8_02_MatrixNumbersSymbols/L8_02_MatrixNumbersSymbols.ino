/*
  VTLabs IoT Training Kit - L8_02 Numbers and Symbols (any matrix)
  ------------------------------------------------------------------
  Purpose:     Show numbers with a tiny 3x5 font (two digits fit on 8x8) and
               8x8 symbols (icons stored as 8 bytes, one per row).
  Hardware:    LED matrix add-on (Tools > LED Matrix (add-on)).
  Libraries:   as L8_01
  Expected:    Counts 0..99 (on wider modules also 100..), then shows each
               icon for 1 s: heart, smile, check, cross, warning, lock, bell...
  Experiments: 1. Design your own icon on squared paper and convert each row
                  to a hex byte (MSB = left pixel). Add it to the list.
               2. Show "-5" or "12%" with showSmallText().
               3. Count down from 10 and beep at 0.
  Troubleshooting:
    - Number too wide for the matrix: showNumber() scrolls it instead.
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>

VTLMatrix matrix;
const uint8_t *icons[] = {vtl_icons::HEART, vtl_icons::SMILE, vtl_icons::CHECK, vtl_icons::CROSS,
                          vtl_icons::WARNING, vtl_icons::LOCK, vtl_icons::UNLOCK, vtl_icons::BELL,
                          vtl_icons::WIFI, vtl_icons::THERMO, vtl_icons::ARROW_UP, vtl_icons::ARROW_DOWN};
// Your own icon - edit these 8 rows:
const uint8_t MY_ICON[8] = {0b00111100, 0b01000010, 0b10100101, 0b10000001,
                            0b10011001, 0b10100101, 0b01000010, 0b00111100};

void setup() {
  matrix.begin();
}

void loop() {
  matrix.setColor(VTL_GREEN);
  for (int n = 0; n <= 99; n++) {
    matrix.showNumber(n);
    delay(120);
  }
  matrix.setColor(VTL_ORANGE);
  for (auto icon : icons) {
    matrix.clear();
    matrix.drawIcon(icon, (matrix.width() - 8) / 2);
    matrix.show();
    delay(1000);
  }
  matrix.clear();
  matrix.drawIcon(MY_ICON, (matrix.width() - 8) / 2);
  matrix.show();
  delay(2000);
  matrix.showSmallText("12%");
  delay(1500);
}
