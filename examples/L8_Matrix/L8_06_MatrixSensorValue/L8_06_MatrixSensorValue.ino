/*
  VTLabs IoT Training Kit - L8_06 Showing Sensor Measurements (any matrix)
  ------------------------------------------------------------------
  Purpose:     Present a measurement three ways and choose the right one:
               number (exact), bar graph (quick to read), scrolling text with
               units (full information).
  Hardware:    LED matrix add-on, DHT22, LDR, POT, SWITCH, BTN1.
  Libraries:   as L8_01, plus "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    BTN1 cycles the view:
                 0  temperature as a number ("25")
                 1  light level as a bar graph (cover the LDR!)
                 2  "T 25.3C  RH 48%  L 62%" scrolling
               SWITCH ON shows the potentiometer instead of light in view 1.
  Experiments: 1. Colour the bar green/yellow/red by level (WS2812).
               2. Show a trend arrow (up/down icon) when the temperature changes.
               3. Which view is best from across the room? Test it.
  Troubleshooting:
    - Number shows "--": DHT22 read failed (see L3_05).
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>
#include <DHT.h>

VTLMatrix matrix;
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
VTLButton btn;
int view = 0;
float t = NAN, h = NAN;

void setup() {
  matrix.begin();
  dht.begin();
  pinMode(VTL_PIN_SWITCH, VTL_PIN_SWITCH_MODE);
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
}

void loop() {
  btn.update();
  if (btn.pressed()) { view = (view + 1) % 3; matrix.clear(); matrix.show(); }

  static unsigned long lastDht = 0;
  if (millis() - lastDht > 2500) {
    lastDht = millis();
    t = dht.readTemperature();
    h = dht.readHumidity();
  }
  int light = map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100);

  static unsigned long lastDraw = 0;
  if (view == 0 && millis() - lastDraw > 500) {
    lastDraw = millis();
    matrix.setColor(VTL_ORANGE);
    if (isnan(t)) matrix.showSmallText("--");
    else matrix.showNumber(lround(t));
  } else if (view == 1 && millis() - lastDraw > 100) {
    lastDraw = millis();
    bool usePot = digitalRead(VTL_PIN_SWITCH) == VTL_ACTIVE_LEVEL(SWITCH);
    int level = usePot ? map(analogRead(VTL_PIN_POT), 0, 4095, 0, 100) : light;
    matrix.clear();
    matrix.setColor(level > 70 ? VTL_YELLOW : VTL_GREEN);
    matrix.drawLevel(level);
    matrix.show();
  } else if (view == 2 && matrix.scrollUpdate()) {
    char buf[48];
    snprintf(buf, sizeof(buf), "T %.1fC  RH %.0f%%  L %d%%", t, h, light);
    matrix.setColor(VTL_CYAN);
    matrix.scrollText(buf);
  }
}
