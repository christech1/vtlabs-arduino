/*
  VTLabs IoT Training Kit - L2_04 Filtering Noisy Readings
  ------------------------------------------------------------------
  Purpose:     Compare raw readings with a moving average and an
               exponential moving average (EMA) on the Serial Plotter.
  Hardware:    LDR (on board). Wave your hand over it.
  Libraries:   none (VTLMovingAverage is in VTLabsKit.h)
  Expected:    Tools > Serial Plotter shows three lines: raw (noisy),
               avg16 (smooth, slower), ema (smooth, adjustable).
  Experiments: 1. Change ALPHA to 0.5 and 0.02. Faster or smoother?
               2. Change the window from 16 to 4 and 64.
               3. Which filter would you use for a light switch? For a display?
  Troubleshooting:
    - Plotter shows nothing: close the Serial Monitor first, select 115200.
  @requires: none
*/
#include <VTLabsKit.h>

const float ALPHA = 0.1;  // EMA weight of the newest sample (0..1)
VTLMovingAverage<16> avg16;
float ema = 0;

void setup() {
  Serial.begin(115200);
  ema = analogRead(VTL_PIN_LDR);
}

void loop() {
  int raw = analogRead(VTL_PIN_LDR);
  float a = avg16.add(raw);
  ema = ALPHA * raw + (1 - ALPHA) * ema;
  Serial.printf("raw:%d,avg16:%.1f,ema:%.1f\n", raw, a, ema);
  delay(20);
}
