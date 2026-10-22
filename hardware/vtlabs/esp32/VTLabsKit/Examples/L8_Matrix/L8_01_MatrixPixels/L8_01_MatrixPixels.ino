/*
  VTLabs IoT Training Kit - L8_01 Individual Pixel Control (any matrix)
  ------------------------------------------------------------------
  Purpose:     Control single LEDs by (x, y) with the VTLMatrix API, which works
               the same on MAX7219 and WS2812 modules. (0,0) is the top-left.
  Hardware:    LED matrix add-on - choose it in Tools > LED Matrix (add-on).
               Wiring: L7_01 (MAX7219) or L7_03 (WS2812).
  Libraries:   MAX7219: "MD_MAX72XX" + "MD_Parola"
               WS2812:  "Adafruit NeoPixel" + "Adafruit NeoMatrix" + "Adafruit GFX Library"
  Expected:    1. Corner pixels light in order: top-left, top-right, bottom-right,
                  bottom-left. (If not, set MATRIX_FLIP_X/Y in the board profile.)
               2. A pixel bounces around the matrix like a ball.
               3. The POT moves a vertical line left and right.
  Experiments: 1. Leave a trail behind the ball (do not clear every frame).
               2. Two balls with different speeds.
               3. Draw your initials pixel by pixel.
  Troubleshooting:
    - Corners in the wrong order: orientation flags - see docs (customising).
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>

VTLMatrix matrix;
int bx = 1, by = 1, dx = 1, dy = 1;

void corner(int x, int y) {
  matrix.clear();
  matrix.setPixel(x, y);
  matrix.show();
  delay(600);
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  Serial.printf("%s matrix, %d x %d\n", matrix.driverName(), matrix.width(), matrix.height());
  const int W = matrix.width() - 1, H = matrix.height() - 1;
  corner(0, 0); corner(W, 0); corner(W, H); corner(0, H);
}

void loop() {
  static unsigned long phaseStart = millis();
  bool potMode = (millis() - phaseStart) % 20000 > 10000;  // 10 s ball, 10 s pot line
  matrix.clear();
  if (!potMode) {
    bx += dx; by += dy;
    if (bx <= 0 || bx >= matrix.width() - 1) dx = -dx;
    if (by <= 0 || by >= matrix.height() - 1) dy = -dy;
    matrix.setColor(VTL_CYAN);
    matrix.setPixel(bx, by);
  } else {
    int x = map(analogRead(VTL_PIN_POT), 0, 4095, 0, matrix.width() - 1);
    matrix.setColor(VTL_YELLOW);
    for (int y = 0; y < matrix.height(); y++) matrix.setPixel(x, y);
  }
  matrix.show();
  delay(80);
}
