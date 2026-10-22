/*
  VTLabs IoT Training Kit - L9_04 Exercise 4: Sensor Measurement on the Matrix
  ------------------------------------------------------------------
  Purpose:     A small "weather station" display that rotates automatically:
               temperature number + trend arrow, humidity number, light bar.
               Colour (WS2812) shows comfort: blue cold, green ok, red hot.
  Hardware:    LED matrix add-on, DHT22, LDR.
  Libraries:   see L8_01, plus "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    Every 3 s the next page: "T" icon then e.g. 24 (with arrow when
               rising/falling by >= 0.3 C), "H" then 51, then a light bar.
               On a 32x8 module the label and value are shown together.
  Task:        Add a "max today" page and reset it with BTN1.
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>
#include <DHT.h>

VTLMatrix matrix;
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
float t = NAN, h = NAN, tPrev = NAN;
int page = 0, trend = 0;

uint32_t comfortColour(float temp) { return temp < 18 ? VTL_BLUE : temp > 28 ? VTL_RED : VTL_GREEN; }

void showLabelled(const char *label, long value, uint32_t colour) {
  char buf[12];
  matrix.setColor(colour);
  if (matrix.width() >= 24) {  // label and value fit side by side
    snprintf(buf, sizeof(buf), "%s %ld", label, value);
    matrix.showSmallText(buf);
  } else {  // 8x8: label first, then value
    matrix.showSmallText(label);
    delay(700);
    matrix.showNumber(value);
  }
}

void setup() {
  matrix.begin();
  dht.begin();
}

void loop() {
  float nt = dht.readTemperature(), nh = dht.readHumidity();
  if (!isnan(nt)) {
    if (!isnan(tPrev)) trend = (nt - tPrev >= 0.3) ? 1 : (tPrev - nt >= 0.3) ? -1 : 0;
    tPrev = t = nt;
    h = nh;
  }
  if (page == 0) {
    if (isnan(t)) { matrix.setColor(VTL_RED); matrix.showSmallText("E"); }
    else {
      showLabelled("C", lround(t), comfortColour(t));
      if (trend) {
        delay(900);
        matrix.clear();
        matrix.drawIcon(trend > 0 ? vtl_icons::ARROW_UP : vtl_icons::ARROW_DOWN, (matrix.width() - 8) / 2);
        matrix.show();
      }
    }
  } else if (page == 1) {
    if (!isnan(h)) showLabelled("H", lround(h), VTL_CYAN);
  } else {
    matrix.clear();
    matrix.setColor(VTL_YELLOW);
    matrix.drawLevel(map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100));
    matrix.show();
  }
  page = (page + 1) % 3;
  delay(3000);  // simple rotating display; also respects the DHT22's 2 s minimum
}
