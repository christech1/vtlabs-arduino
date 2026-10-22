/*
  VTLabs IoT Training Kit - L7_03 WS2812 Matrix: Data Order and Pixel Mapping
  ------------------------------------------------------------------
  Purpose:     WS2812 ("NeoPixel") LEDs each contain a tiny controller. They are
               chained on ONE data wire: the ESP32 sends 24 bits per LED with
               precise timing (~1.25 us per bit, 800 kHz); each LED keeps its
               24 bits and passes the rest on. So the panel is really a STRIP -
               and you must know how the strip snakes through the 8x8 grid.
  Hardware:    WS2812 8x8 add-on - Tools > LED Matrix (add-on) > WS2812 ...
               Wiring:  5V -> 5V supply (NOT the ESP32 3V3 pin)   GND -> GND (shared!)
                        DIN -> 330 ohm -> VTL_PIN_WS2812_DIN (GPIO13)
               1000 uF capacitor across 5V/GND at the panel. A 74AHCT125 level
               shifter (3.3 V -> 5 V) is recommended: WS2812 at 5 V needs
               > 3.5 V for a reliable HIGH, the ESP32 gives 3.3 V.
  Libraries:   "Adafruit NeoPixel" (>= 1.12)
  Expected:    1. Pixels light one by one in STRIP order 0..63, so you can see
                  whether rows are "progressive" (all left->right) or
                  "zigzag/serpentine" (direction alternates each row).
               2. Whole panel RED, then GREEN, then BLUE for 1 s each.
                  If the colours come out wrong, the COLOUR ORDER setting is wrong.
  Experiments: 1. Write down which corner pixel 0 is in, and the row pattern.
                  Put the answer in the board profile (WS2812_LAYOUT).
               2. Change WS2812_COLOR_ORDER to NEO_RGB and observe step 2.
               3. Write xyToIndex(x, y) for a zigzag panel yourself.
  Troubleshooting:
    - First LED random colour, rest dark: no common GND, or data level too low
      (add the level shifter, or power the panel at ~4.5 V).
    - Flicker / ESP32 resets when many LEDs are on: supply too weak - see L7_04.
  @requires: matrix-ws2812
*/
#include <VTLabsKit.h>
#include <Adafruit_NeoPixel.h>

#if !VTL_HAS_MATRIX || VTL_MATRIX_DRIVER != VTL_MATRIX_DRIVER_WS2812
#error "This lesson is for WS2812 matrices. Select a WS2812 module in Tools > LED Matrix (add-on)."
#endif

const int NUM = VTL_MATRIX_WIDTH * VTL_MATRIX_HEIGHT;
Adafruit_NeoPixel strip(NUM, VTL_PIN_WS2812_DIN, VTL_WS2812_COLOR_ORDER + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(VTL_WS2812_MAX_BRIGHTNESS / 2);
  strip.clear();
  strip.show();
}

void loop() {
  strip.clear();
  for (int i = 0; i < NUM; i++) {  // 1. strip order
    strip.setPixelColor(i, strip.Color(0, 0, 80));
    if (i > 0) strip.setPixelColor(i - 1, strip.Color(0, 30, 0));  // trail
    strip.show();
    Serial.printf("pixel %d\n", i);
    delay(120);
  }
  const uint32_t colours[3] = {strip.Color(255, 0, 0), strip.Color(0, 255, 0), strip.Color(0, 0, 255)};
  const char *names[3] = {"RED", "GREEN", "BLUE"};
  for (int c = 0; c < 3; c++) {  // 2. colour order test
    strip.fill(colours[c]);
    strip.show();
    Serial.printf("Panel should be %s\n", names[c]);
    delay(1000);
  }
}
