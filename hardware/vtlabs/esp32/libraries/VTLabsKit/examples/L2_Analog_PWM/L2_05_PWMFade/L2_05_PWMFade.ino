/*
  VTLabs IoT Training Kit - L2_05 Dim an LED with PWM
  ------------------------------------------------------------------
  Purpose:     Pulse-width modulation: switch fast, vary the ON time.
               Uses the ESP32 core 3.x LEDC API: ledcAttach() / ledcWrite().
  Hardware:    LED_YELLOW (on board).
  Libraries:   none
  Expected:    The yellow LED slowly fades up and down (breathing).
  Experiments: 1. Set PWM_FREQ to 5 Hz. What do you see now? Why?
               2. Use 10-bit resolution (0..1023) and adjust the loop.
               3. Our eyes are not linear: try duty = (i*i)/255 for a
                  smoother-looking fade (gamma correction).
  Troubleshooting:
    - "ledcSetup was not declared": that is the old 2.x API; this kit uses 3.x.
  @requires: none
*/
#include <VTLabsKit.h>

const uint32_t PWM_FREQ = 5000;  // Hz
const uint8_t PWM_BITS = 8;      // duty 0..255

void setup() {
  ledcAttach(VTL_PIN_LED_YELLOW, PWM_FREQ, PWM_BITS);
}

void loop() {
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(VTL_PIN_LED_YELLOW, duty);
    delay(6);
  }
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(VTL_PIN_LED_YELLOW, duty);
    delay(6);
  }
}
