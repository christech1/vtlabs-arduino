/*
  VTLabs IoT Training Kit - L5_01 Project: Automatic Lighting
  ------------------------------------------------------------------
  Purpose:     Combine sensors, decisions and an actuator:
               the "lamp" (LED_YELLOW, PWM) turns on when it is dark AND
               someone is present, fades smoothly, and stays on for a while.
               Uses hysteresis so it does not flicker at the threshold.
  Hardware:    LDR, PIR, LED_YELLOW, SWITCH (manual override), POT (threshold).
  Libraries:   none
  Expected:    - Dark + motion -> lamp fades on, stays on 20 s after last motion.
               - Bright -> lamp off.
               - SWITCH ON -> lamp always on (manual mode).
               - POT sets the darkness threshold (printed on Serial).
  Experiments: 1. Remove the hysteresis (ON_BAND = 0). What happens at dusk?
               2. Dim the lamp to 20 % instead of off when nobody is there.
               3. Use the relay for a real 12 V lamp.
  Troubleshooting:
    - Lamp never turns on: cover the LDR and check "dark=1" on Serial.
  @requires: none
*/
#include <VTLabsKit.h>

const unsigned long HOLD_MS = 20000;
const int ON_BAND = 150;  // hysteresis in ADC counts
int brightness = 0;
bool dark = false;
unsigned long lastMotion = 0;

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_PIR, VTL_PIN_PIR_MODE);
  pinMode(VTL_PIN_SWITCH, VTL_PIN_SWITCH_MODE);
  ledcAttach(VTL_PIN_LED_YELLOW, 5000, 8);
}

void loop() {
  int threshold = map(analogRead(VTL_PIN_POT), 0, 4095, 200, 3000);
  int light = analogRead(VTL_PIN_LDR);
  if (!dark && light < threshold - ON_BAND) dark = true;   // gets dark
  if (dark && light > threshold + ON_BAND) dark = false;   // gets bright
  if (digitalRead(VTL_PIN_PIR) == HIGH) lastMotion = millis();
  bool present = millis() - lastMotion < HOLD_MS && lastMotion != 0;
  bool manual = digitalRead(VTL_PIN_SWITCH) == VTL_ACTIVE_LEVEL(SWITCH);

  int target = (manual || (dark && present)) ? 255 : 0;
  if (brightness < target) brightness++;
  if (brightness > target) brightness--;
  ledcWrite(VTL_PIN_LED_YELLOW, brightness);

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.printf("light=%d threshold=%d dark=%d present=%d manual=%d lamp=%d\n", light, threshold, dark,
                  present, manual, brightness);
  }
  delay(8);  // fade speed
}
