/*
  VTLabs IoT Training Kit - L8_07 System Status and Warnings (any matrix)
  ------------------------------------------------------------------
  Purpose:     Use the matrix as a status panel: one icon per state, blinking
               for urgency, priority when several problems happen at once.
  Hardware:    LED matrix add-on, LDR, POT, DHT22, PIR, BUZZER.
  Libraries:   as L8_01, plus "DHT sensor library" + "Adafruit Unified Sensor"
  States (highest priority first):
               SENSOR FAULT  cross, fast blink       (DHT22 read failed)
               OVER-TEMP     warning, blink + beep   (T above POT-set limit 20..40 C)
               DARK          arrow down              (light < 10 %)
               MOTION        bell                    (PIR active)
               OK            check, steady
  Expected:    The matrix always shows the most important current state; the
               Serial Monitor prints state changes.
  Experiments: 1. Add a "Wi-Fi down" state with the WIFI icon (combine with L4_01).
               2. Acknowledge the beep with BTN1 (see L9_06).
               3. Show the number of active problems in the corner.
  Troubleshooting:
    - Stays on SENSOR FAULT: DHT22 not connected or wrong pin.
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>
#include <DHT.h>

VTLMatrix matrix;
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);

enum Status { ST_OK, ST_MOTION, ST_DARK, ST_OVERTEMP, ST_FAULT };
const char *statusName[] = {"OK", "MOTION", "DARK", "OVER-TEMP", "SENSOR FAULT"};
Status status = ST_OK;
float t = NAN;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  dht.begin();
  pinMode(VTL_PIN_PIR, VTL_PIN_PIR_MODE);
}

void loop() {
  static unsigned long lastDht = 0;
  if (millis() - lastDht > 2500) { lastDht = millis(); t = dht.readTemperature(); }
  float limit = map(analogRead(VTL_PIN_POT), 0, 4095, 20, 40);
  int light = map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100);

  Status s = ST_OK;  // evaluate from lowest to highest priority
  if (digitalRead(VTL_PIN_PIR)) s = ST_MOTION;
  if (light < 10) s = ST_DARK;
  if (!isnan(t) && t > limit) s = ST_OVERTEMP;
  if (isnan(t) && millis() > 5000) s = ST_FAULT;
  if (s != status) {
    status = s;
    Serial.printf("Status: %s (T=%.1f limit=%.0f light=%d%%)\n", statusName[s], t, limit, light);
  }

  bool blinkOn = true;
  if (status == ST_FAULT) blinkOn = (millis() / 150) % 2;
  if (status == ST_OVERTEMP) blinkOn = (millis() / 400) % 2;
  const uint8_t *icon[] = {vtl_icons::CHECK, vtl_icons::BELL, vtl_icons::ARROW_DOWN, vtl_icons::WARNING,
                           vtl_icons::CROSS};
  const uint32_t colour[] = {VTL_GREEN, VTL_CYAN, VTL_BLUE, VTL_ORANGE, VTL_RED};
  matrix.clear();
  matrix.setColor(colour[status]);
  if (blinkOn) matrix.drawIcon(icon[status], (matrix.width() - 8) / 2);
  matrix.show();

  static unsigned long lastBeep = 0;
  if (status == ST_OVERTEMP && millis() - lastBeep > 2000) { lastBeep = millis(); tone(VTL_PIN_BUZZER, 2200, 120); }
  delay(30);
}
