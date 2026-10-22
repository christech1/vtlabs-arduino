/*
  VTLabs IoT Training Kit - L8_03 Predefined Patterns (any matrix)
  ------------------------------------------------------------------
  Purpose:     Generate patterns with loops and simple maths instead of storing
               every pixel: borders, stripes, checkerboard, diagonals, circles.
  Hardware:    LED matrix add-on (Tools > LED Matrix (add-on)). BTN1 = next pattern.
  Libraries:   as L8_01
  Expected:    BTN1 cycles through 7 patterns; the name is printed on Serial.
  Experiments: 1. Add a "plus sign" pattern centred on the matrix.
               2. Make the circle radius follow the POT.
               3. Invert any pattern (on <-> off).
  Troubleshooting:
    - Circle looks oval on 32x8: the module is not square - that is expected.
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>

VTLMatrix matrix;
VTLButton btn;
int pattern = 0;
const char *names[] = {"border", "horizontal stripes", "vertical stripes", "checkerboard",
                       "diagonal", "X", "circle"};

bool pixelOn(int p, int x, int y, int W, int H) {
  switch (p) {
    case 0: return x == 0 || y == 0 || x == W - 1 || y == H - 1;
    case 1: return y % 2 == 0;
    case 2: return x % 2 == 0;
    case 3: return (x + y) % 2 == 0;
    case 4: return (x % H) == y;
    case 5: return (x % H) == y || (x % H) == H - 1 - y;
    default: {
      float cx = (W - 1) / 2.0, cy = (H - 1) / 2.0, r = H / 2.0 - 0.5;
      float d = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
      return fabs(d - r) < 0.6;
    }
  }
}

void draw() {
  matrix.clear();
  for (int y = 0; y < matrix.height(); y++)
    for (int x = 0; x < matrix.width(); x++)
      if (pixelOn(pattern, x, y, matrix.width(), matrix.height())) matrix.setPixel(x, y);
  matrix.show();
  Serial.printf("Pattern %d: %s\n", pattern, names[pattern]);
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.setColor(VTL_BLUE);
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  draw();
}

void loop() {
  btn.update();
  if (btn.pressed()) {
    pattern = (pattern + 1) % 7;
    draw();
  }
}
