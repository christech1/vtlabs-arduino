/*
  VTLabs IoT Training Kit - L3_01 Diagnostics over USB Serial
  ------------------------------------------------------------------
  Purpose:     Use Serial as a debugging tool: board report, formatted
               values, timing, and the ESP32 log levels.
  Hardware:    Only the USB cable. Reads POT, LDR, BTN1, SWITCH.
  Libraries:   none
  Expected:    Board info once, then every second a status line:
               "t=12s heap=290123 pot=1876 ldr=2210 btn1=0 switch=1".
               Set Tools > Core Debug Level = Info to see log_i() messages.
  Experiments: 1. Add the loop time (how many loop() runs per second).
               2. Print a warning (log_w) when the LDR is very dark.
               3. Compare Serial.print, Serial.printf and log_i output.
  Troubleshooting:
    - Garbage characters: Serial Monitor baud must be 115200.
    - Nothing at all after upload: press EN (reset) with the monitor open.
  @requires: none
*/
#include <VTLabsKit.h>

void setup() {
  Serial.begin(115200);
  delay(300);
  vtlPrintBoardInfo(Serial);
  Serial.printf("Chip: %s rev %d, %d cores, %lu MHz\n", ESP.getChipModel(), ESP.getChipRevision(),
                ESP.getChipCores(), (unsigned long)ESP.getCpuFreqMHz());
  Serial.printf("Flash: %lu bytes, sketch %lu bytes\n", (unsigned long)ESP.getFlashChipSize(),
                (unsigned long)ESP.getSketchSize());
  pinMode(VTL_PIN_BTN1, VTL_PIN_BTN1_MODE);
  pinMode(VTL_PIN_SWITCH, VTL_PIN_SWITCH_MODE);
  log_i("setup finished");  // shown only if Core Debug Level >= Info
}

void loop() {
  static unsigned long last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    Serial.printf("t=%lus heap=%lu pot=%d ldr=%d btn1=%d switch=%d\n", millis() / 1000,
                  (unsigned long)ESP.getFreeHeap(), analogRead(VTL_PIN_POT), analogRead(VTL_PIN_LDR),
                  digitalRead(VTL_PIN_BTN1) == VTL_ACTIVE_LEVEL(BTN1),
                  digitalRead(VTL_PIN_SWITCH) == VTL_ACTIVE_LEVEL(SWITCH));
  }
}
