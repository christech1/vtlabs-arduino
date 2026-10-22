/*
  VTLabs IoT Training Kit - L2_06 ADC vs PWM vs true DAC
  ------------------------------------------------------------------
  Purpose:     The potentiometer (ADC input) sets two outputs at once:
               - LED_YELLOW by PWM  (pin switches 0 V / 3.3 V very fast)
               - LED_GREEN  by DAC2 (pin outputs a real steady voltage)
  Hardware:    POT, LED_YELLOW, LED_GREEN (GPIO26 = DAC2 on the classic ESP32).
               A multimeter (DC volts) and, if available, an oscilloscope.
  Libraries:   none
  Expected:    Both LEDs follow the knob. A multimeter shows a similar average
               voltage on both pins, but an oscilloscope shows a square wave on
               the PWM pin and a flat line on the DAC pin. The DAC LED stays OFF
               for low values because an LED needs about 1.8 V to light.
  Experiments: 1. Measure both pins with a multimeter at 25 %, 50 %, 75 %.
               2. Why does the DAC LED start lighting only above ~55 %?
               3. Which output would you use for an audio signal? For a motor?
  Troubleshooting:
    - DAC LED never lights: check that LED_GREEN is on GPIO25 or 26 (DAC pins).
  @requires: none
*/
#include <VTLabsKit.h>

#if VTL_PIN_LED_GREEN != 25 && VTL_PIN_LED_GREEN != 26  /* DAC1 / DAC2 on the classic ESP32 */
#error "This lesson needs LED_GREEN on a DAC pin (GPIO25 or GPIO26)."
#endif

void setup() {
  Serial.begin(115200);
  ledcAttach(VTL_PIN_LED_YELLOW, 5000, 8);
}

void loop() {
  int raw = analogRead(VTL_PIN_POT);  // 0..4095
  uint8_t level = raw >> 4;           // 0..255
  ledcWrite(VTL_PIN_LED_YELLOW, level);  // PWM: average = level/255 * 3.3 V
  dacWrite(VTL_PIN_LED_GREEN, level);    // DAC: steady  ~ level/255 * 3.3 V
  Serial.printf("ADC %4d -> output level %3d -> expected %.2f V\n", raw, level, level * 3.3 / 255);
  delay(200);
}
