/*
  VTLabs IoT Training Kit - L7_04 WS2812: Brightness, Power and Gamma
  ------------------------------------------------------------------
  Purpose:     Each WS2812 draws up to ~60 mA at full white (20 mA per colour).
               64 LEDs x 60 mA = 3.84 A - far more than USB (0.5-0.9 A) or the
               ESP32 board's regulator can give. This lesson estimates the current
               and shows why the kit caps brightness (VTL_WS2812_MAX_BRIGHTNESS).
  Hardware:    WS2812 8x8 add-on (wiring in L7_03). Optional: USB power meter.
  Libraries:   "Adafruit NeoPixel" (>= 1.12)
  Expected:    The POT sets brightness (0..cap). The Serial Monitor prints the
               estimated current for the pattern shown. BTN1 switches between
               patterns: 1 LED, 8 LEDs, full panel white.
               Gamma-corrected fade on the top row looks smoother than linear.
  Experiments: 1. With a USB power meter, compare measured vs estimated current.
               2. Why does white cost 3x the current of pure red?
               3. Calculate the supply (A) for a 16x16 panel at 30 % brightness.
  Troubleshooting:
    - ESP32 resets when the panel goes white: supply too weak; lower the
      brightness cap in the board profile or use a separate 5 V / 4 A supply.
  @requires: matrix-ws2812
*/
#include <VTLabsKit.h>
#include <Adafruit_NeoPixel.h>

#if !VTL_HAS_MATRIX || VTL_MATRIX_DRIVER != VTL_MATRIX_DRIVER_WS2812
#error "This lesson is for WS2812 matrices. Select a WS2812 module in Tools > LED Matrix (add-on)."
#endif

const int NUM = VTL_MATRIX_WIDTH * VTL_MATRIX_HEIGHT;
const float MA_PER_CHANNEL = 20.0;  // datasheet typical at full duty
Adafruit_NeoPixel strip(NUM, VTL_PIN_WS2812_DIN, VTL_WS2812_COLOR_ORDER + NEO_KHZ800);
VTLButton btn;
int pattern = 0;

void setup() {
  Serial.begin(115200);
  strip.begin();
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
}

void loop() {
  btn.update();
  if (btn.pressed()) pattern = (pattern + 1) % 3;
  uint8_t b = map(analogRead(VTL_PIN_POT), 0, 4095, 0, VTL_WS2812_MAX_BRIGHTNESS);
  strip.setBrightness(b);
  strip.clear();
  int lit = pattern == 0 ? 1 : pattern == 1 ? 8 : NUM;
  for (int i = 0; i < lit; i++) strip.setPixelColor(i, strip.Color(255, 255, 255));
  if (pattern != 2) {  // top-row demo: linear vs gamma-corrected fade
    for (int x = 0; x < VTL_MATRIX_WIDTH; x++) {
      uint8_t v = x * 255 / (VTL_MATRIX_WIDTH - 1);
      strip.setPixelColor(NUM - VTL_MATRIX_WIDTH + x, strip.gamma32(strip.Color(0, v, 0)));
    }
  }
  strip.show();

  static unsigned long last = 0;
  if (millis() - last > 1000) {
    last = millis();
    float ma = lit * 3 * MA_PER_CHANNEL * b / 255.0 + NUM * 1.0;  // + ~1 mA idle per LED
    Serial.printf("pattern %d: %d white LEDs at brightness %d/255 -> about %.0f mA\n", pattern, lit, b, ma);
  }
  delay(20);
}
