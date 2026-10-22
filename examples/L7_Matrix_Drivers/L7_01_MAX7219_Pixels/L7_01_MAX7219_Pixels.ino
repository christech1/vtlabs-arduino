/*
  VTLabs IoT Training Kit - L7_01 MAX7219 LED Matrix: Pixels and Rows
  ------------------------------------------------------------------
  Purpose:     Learn how the MAX7219 driver works, using its own library.
               The MAX7219 is a single-colour LED driver: it stores 8 bytes
               (one per row of 8 LEDs) and multiplexes them itself. The ESP32
               only sends 16-bit commands over SPI. Several modules are
               daisy-chained: data shifts from one MAX7219 to the next.
  Hardware:    MAX7219 module add-on - Tools > LED Matrix (add-on) > MAX7219 ...
               Wiring (5 V module, 3.3 V logic usually works):
                 VCC -> 5V   GND -> GND
                 DIN -> VTL_PIN_SPI_MOSI (GPIO23)   CLK -> VTL_PIN_SPI_SCK (GPIO18)
                 CS/LOAD -> VTL_PIN_MAX7219_CS (GPIO15)
               Module type matters: FC-16 (common blue 4-in-1), GENERIC, PAROLA,
               ICSTATION wire the LEDs differently. Set it in the board profile
               (MAX7219_HW_TYPE). Wrong type = mirrored or rotated image.
  Libraries:   "MD_MAX72XX" by majicDesigns (>= 3.5)
  Expected:    1. One pixel walks along every row.  2. Rows fill from top.
               3. Brightness ramps 0..15.  4. A test pattern; then repeat.
  Experiments: 1. Note where pixel (row 0, col 0) appears. The library counts
                  columns from the RIGHT - the VTLMatrix API (L8_*) hides this.
               2. Try the other module types and describe what changes.
               3. Use mx.transform(MD_MAX72XX::TSL) to shift the image left.
  Troubleshooting:
    - Nothing lights: check CS pin and 5 V supply; all LEDs on = DIN/CLK swapped.
    - Image in the wrong order across modules: wrong module type, or the
      chain is connected from the other end (DIN goes in on the right side).
  @requires: matrix-max7219
*/
#include <VTLabsKit.h>
#include <SPI.h>
#include <MD_MAX72xx.h>

#if !VTL_HAS_MATRIX || VTL_MATRIX_DRIVER != VTL_MATRIX_DRIVER_MAX7219
#error "This lesson is for MAX7219 matrices. Select a MAX7219 module in Tools > LED Matrix (add-on)."
#endif

MD_MAX72XX mx(VTL_MAX7219_HW_TYPE, SPI, VTL_PIN_MAX7219_CS, VTL_MAX7219_DEVICES);

void setup() {
  Serial.begin(115200);
  SPI.begin(VTL_PIN_SPI_SCK, VTL_PIN_SPI_MISO, VTL_PIN_SPI_MOSI);
  if (!mx.begin()) Serial.println("MD_MAX72XX begin failed");
  mx.control(MD_MAX72XX::INTENSITY, 2);
  Serial.printf("%d device(s), %d columns\n", mx.getDeviceCount(), mx.getColumnCount());
}

void loop() {
  mx.clear();
  for (uint8_t r = 0; r < 8; r++) {  // 1. walking pixel
    for (uint16_t c = 0; c < mx.getColumnCount(); c++) {
      mx.setPoint(r, c, true);
      delay(15);
      mx.setPoint(r, c, false);
    }
  }
  for (uint8_t r = 0; r < 8; r++) {  // 2. fill rows (0xFF = all 8 LEDs of a device row)
    mx.setRow(r, 0xFF);
    delay(150);
  }
  for (uint8_t i = 0; i <= MAX_INTENSITY; i++) {  // 3. brightness
    mx.control(MD_MAX72XX::INTENSITY, i);
    delay(120);
  }
  mx.control(MD_MAX72XX::INTENSITY, 2);
  mx.clear();
  for (uint16_t c = 0; c < mx.getColumnCount(); c++)  // 4. checkerboard
    mx.setColumn(c, (c % 2) ? 0xAA : 0x55);
  delay(1500);
}
