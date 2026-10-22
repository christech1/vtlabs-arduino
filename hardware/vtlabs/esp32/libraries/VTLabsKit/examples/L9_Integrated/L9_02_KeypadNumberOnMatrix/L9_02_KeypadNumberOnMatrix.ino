/*
  VTLabs IoT Training Kit - L9_02 Exercise 2: Keypad Value on the LED Matrix
  ------------------------------------------------------------------
  Purpose:     Show what the user types, live, and confirm the entered value.
  Hardware:    Matrix keypad add-on + LED matrix add-on (Tools menu: both).
  Libraries:   see L8_01
  Keys:        digits = type (max 6)   * = delete   hold * = clear   # = confirm
               A = add the value to a running total, B = show the total
  Expected:    While typing, the last digits are shown (8x8: last 2 digits;
               32x8: up to 8). '#' scrolls "= 1234" and the value is printed.
               'B' scrolls "SUM 5678".
  Task:        Add a calculator: A = +, B = -, C = x, D = equals.
  @requires: keypad, matrix
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <VTLabsMatrix.h>

VTLKeypad keypad;
VTLMatrix matrix;
VTLLineInput input;
long total = 0;

void preview() {
  const char *t = input.text();
  int fit = (matrix.width() + 1) / 4;  // characters of 3x5 font that fit
  int len = strlen(t);
  matrix.setColor(VTL_GREEN);
  if (len == 0) { matrix.clear(); matrix.setPixel(0, matrix.height() - 1); matrix.show(); return; }  // cursor
  matrix.showSmallText(t + (len > fit ? len - fit : 0));
}

void setup() {
  Serial.begin(115200);
  keypad.begin();
  matrix.begin();
  input.begin(6);
  preview();
}

void loop() {
  matrix.scrollUpdate();
  VTLKeyEvent e = keypad.update();
  if (e.type == VTL_KEY_HELD && e.key == '*') { input.clear(); preview(); return; }
  if (e.type != VTL_KEY_PRESSED) return;
  tone(VTL_PIN_BUZZER, 3000, 10);

  if (e.key == 'A' && input.length()) {
    total += input.value();
    Serial.printf("added %ld, total %ld\n", input.value(), total);
    input.clear();
    matrix.setColor(VTL_CYAN);
    matrix.scrollText("+");
    return;
  }
  if (e.key == 'B') {
    char buf[24];
    snprintf(buf, sizeof(buf), "SUM %ld", total);
    matrix.setColor(VTL_ORANGE);
    matrix.scrollText(buf);
    return;
  }
  if (input.feed(e.key)) {
    char buf[24];
    snprintf(buf, sizeof(buf), "= %ld", input.value());
    Serial.printf("Entered %ld\n", input.value());
    matrix.setColor(VTL_YELLOW);
    matrix.scrollText(buf);
    input.clear();
    return;
  }
  preview();
}
