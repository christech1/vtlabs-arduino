/*
  VTLabs IoT Training Kit - L2_03 From Raw ADC Counts to Engineering Units
  ------------------------------------------------------------------
  Purpose:     Turn raw readings into meaningful units:
               - POT: millivolts using the ESP32's factory calibration
                 (analogReadMilliVolts) compared with the naive formula.
               - LDR: 0..100 % light using a two-point calibration you record.
  Hardware:    POT, LDR (on board).
  Libraries:   none
  Expected:    "pot: raw 2048  naive 1650 mV  calibrated 1702 mV" and
               "light: 64 %". Type 'd' (dark) or 'b' (bright) in the Serial
               Monitor to store the current LDR reading as a calibration point.
  Experiments: 1. Measure the pot wiper with a multimeter. Which mV value is closer?
               2. Calibrate the LDR in the dark (covered) and under a torch.
               3. Why is a linear map only an approximation for an LDR?
  Troubleshooting:
    - Percent stays 0 or 100: calibrate dark and bright points first.
  @requires: none
*/
#include <VTLabsKit.h>

int ldrDarkMv = 100;     // replace by pressing 'd' with the LDR covered
int ldrBrightMv = 3000;  // replace by pressing 'b' under bright light

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);  // full 0..~3.1 V input range
  Serial.println("Type d = store DARK point, b = store BRIGHT point");
}

void loop() {
  int raw = analogRead(VTL_PIN_POT);
  int naiveMv = (long)raw * VTL_ADC_VREF_MV / 4095;
  int calMv = analogReadMilliVolts(VTL_PIN_POT);

  int ldrMv = analogReadMilliVolts(VTL_PIN_LDR);
  int percent = constrain(map(ldrMv, ldrDarkMv, ldrBrightMv, 0, 100), 0, 100);

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'd') { ldrDarkMv = ldrMv; Serial.printf("DARK point = %d mV\n", ldrMv); }
    if (c == 'b') { ldrBrightMv = ldrMv; Serial.printf("BRIGHT point = %d mV\n", ldrMv); }
  }

  Serial.printf("pot: raw %4d  naive %4d mV  calibrated %4d mV   |   light: %3d %% (%d mV)\n",
                raw, naiveMv, calMv, percent, ldrMv);
  delay(500);
}
