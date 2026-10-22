/*
  VTLabs IoT Training Kit - L7_05 WS2812: x/y Drawing and Text with NeoMatrix
  ------------------------------------------------------------------
  Purpose:     Adafruit NeoMatrix converts (x, y) into the strip index using the
               layout you discovered in L7_03, and adds Adafruit GFX drawing:
               lines, rectangles, text in colour.
  Hardware:    WS2812 8x8 add-on (wiring in L7_03).
  Libraries:   "Adafruit NeoMatrix" + "Adafruit NeoPixel" + "Adafruit GFX Library"
  Expected:    A colour frame and diagonal, then "VTLabs IoT" scrolls in
               changing colours.
  Experiments: 1. If the frame looks broken, the layout flags in the board profile
                  (WS2812_LAYOUT) do not match your panel - fix them.
               2. Draw a smiley with drawPixel().
               3. Scroll the temperature from the DHT22.
  Troubleshooting:
    - Text mirrored: add/remove NEO_MATRIX_RIGHT or NEO_MATRIX_BOTTOM in the layout.
  @requires: matrix-ws2812
*/
#include <VTLabsKit.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#if !VTL_HAS_MATRIX || VTL_MATRIX_DRIVER != VTL_MATRIX_DRIVER_WS2812
#error "This lesson is for WS2812 matrices. Select a WS2812 module in Tools > LED Matrix (add-on)."
#endif

Adafruit_NeoMatrix matrix(VTL_MATRIX_WIDTH, VTL_MATRIX_HEIGHT, VTL_PIN_WS2812_DIN, VTL_WS2812_LAYOUT,
                          VTL_WS2812_COLOR_ORDER + NEO_KHZ800);
const char *TEXT = "VTLabs IoT";
int x;
uint8_t hue = 0;

void setup() {
  matrix.begin();
  matrix.setBrightness(VTL_WS2812_MAX_BRIGHTNESS / 2);
  matrix.setTextWrap(false);
  matrix.fillScreen(0);
  matrix.drawRect(0, 0, matrix.width(), matrix.height(), matrix.Color(0, 0, 255));
  matrix.drawLine(0, 0, matrix.width() - 1, matrix.height() - 1, matrix.Color(255, 0, 0));
  matrix.show();
  delay(2000);
  x = matrix.width();
}

void loop() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  uint32_t c = Adafruit_NeoPixel::ColorHSV(hue * 256);  // 0xRRGGBB
  matrix.setTextColor(matrix.Color((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF));
  matrix.print(TEXT);
  matrix.show();
  if (--x < -(int)(strlen(TEXT) * 6)) { x = matrix.width(); hue += 40; }
  delay(70);
}
