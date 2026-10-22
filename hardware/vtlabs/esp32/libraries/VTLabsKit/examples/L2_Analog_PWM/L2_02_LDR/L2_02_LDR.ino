/*
  VTLabs IoT Training Kit - L2_02 Light Sensor (LDR)
  ------------------------------------------------------------------
  Purpose:     Read a light-dependent resistor through a voltage divider.
  Hardware:    LDR + 10k resistor divider -> VTL_PIN_LDR (ADC1).
               Brighter light = lower LDR resistance = higher voltage.
  Libraries:   none
  Expected:    Value rises in bright light, falls when you cover the LDR.
               "DARK" / "DIM" / "BRIGHT" is printed.
  Experiments: 1. Adjust the thresholds for your classroom.
               2. Turn on LED_YELLOW when it is DARK (preview of L5_01).
               3. Shine the phone torch from 5 cm and 30 cm. Record the values.
  Troubleshooting:
    - Value goes DOWN in light: the divider is the other way round on your
      board - swap the comparison or note it in the board profile.
  @requires: none
*/
#include <VTLabsKit.h>

const int DARK_BELOW = 800;
const int BRIGHT_ABOVE = 2500;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int light = analogRead(VTL_PIN_LDR);
  const char *level = light < DARK_BELOW ? "DARK" : light > BRIGHT_ABOVE ? "BRIGHT" : "DIM";
  Serial.printf("light:%d  (%s)\n", light, level);
  delay(250);
}
