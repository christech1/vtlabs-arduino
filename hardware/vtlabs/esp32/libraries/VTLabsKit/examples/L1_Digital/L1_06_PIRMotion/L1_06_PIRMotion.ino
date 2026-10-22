/*
  VTLabs IoT Training Kit - L1_06 Digital Sensor: PIR Motion
  ------------------------------------------------------------------
  Purpose:     Read a sensor that gives a digital (HIGH/LOW) output.
  Hardware:    PIR motion sensor HC-SR501 on VTL_PIN_PIR, LED_YELLOW.
               The sensor needs ~30-60 s after power-up to settle.
  Libraries:   none
  Expected:    Motion -> yellow LED on and "Motion detected" with a counter.
               No motion -> "Motion ended" after the sensor's hold time.
  Experiments: 1. Turn the PIR's time potentiometer: how does "hold time" change?
               2. Print how long each motion event lasted (use millis()).
               3. Only count motion while SWITCH is ON ("armed").
  Troubleshooting:
    - Always HIGH after reset: wait for the warm-up time.
    - Random triggers: keep the sensor away from heaters, sun and moving air.
  @requires: none
*/
#include <VTLabsKit.h>

int lastMotion = LOW;
unsigned long events = 0;

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_PIR, VTL_PIN_PIR_MODE);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  Serial.println("PIR warming up - wait 30 s for stable readings");
}

void loop() {
  int motion = digitalRead(VTL_PIN_PIR);
  if (motion != lastMotion) {
    lastMotion = motion;
    digitalWrite(VTL_PIN_LED_YELLOW, motion);
    if (motion == HIGH) {
      events++;
      Serial.printf("Motion detected (#%lu) at %lu s\n", events, millis() / 1000);
    } else {
      Serial.println("Motion ended");
    }
  }
  delay(20);
}
