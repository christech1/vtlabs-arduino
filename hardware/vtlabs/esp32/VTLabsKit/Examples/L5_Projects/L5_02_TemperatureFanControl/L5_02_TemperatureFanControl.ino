/*
  VTLabs IoT Training Kit - L5_02 Project: Temperature-Based Fan Control
  ------------------------------------------------------------------
  Purpose:     Two control methods side by side:
               1) ON/OFF with hysteresis -> RELAY (fan on/off)
               2) Proportional -> LED_RED brightness = simulated fan speed (PWM)
  Hardware:    DHT22, RELAY (small 12 V fan or lamp - LOW VOLTAGE ONLY),
               LED_RED, POT (sets the target temperature 20..35 C).
  Libraries:   "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    Serial: "T=27.4 target=26.0 relay=ON speed=45%".
               Relay ON above target+0.5 C, OFF below target-0.5 C.
               If the sensor fails 3 times in a row, the fan is forced ON
               (fail-safe for cooling) and the buzzer beeps.
  Experiments: 1. Warm the sensor with your fingers and watch both outputs.
               2. Change HYST to 0. How often does the relay switch?
               3. Is "fail-safe = ON" right for a heater? What would you choose?
  Troubleshooting:
    - Relay chatters: increase HYST.
  @requires: none
*/
#include <VTLabsKit.h>
#include <DHT.h>

const float HYST = 0.5;     // +/- degrees C around the target
const float FULL_SPEED_ABOVE = 5.0;  // proportional band in C
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
bool fanOn = false;
int failures = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(VTL_PIN_RELAY, OUTPUT);
  vtlRelayWrite(false);
  ledcAttach(VTL_PIN_LED_RED, 5000, 8);
}

void loop() {
  delay(2500);
  float target = map(analogRead(VTL_PIN_POT), 0, 4095, 200, 350) / 10.0;
  float t = dht.readTemperature();
  if (isnan(t)) {
    if (++failures >= 3) {
      fanOn = true;
      vtlRelayWrite(true);
      ledcWrite(VTL_PIN_LED_RED, 255);
      tone(VTL_PIN_BUZZER, 3000, 100);
      Serial.println("SENSOR FAULT - fan forced ON");
    }
    return;
  }
  failures = 0;
  if (!fanOn && t > target + HYST) fanOn = true;
  if (fanOn && t < target - HYST) fanOn = false;
  vtlRelayWrite(fanOn);

  float error = t - target;
  int speed = constrain((int)(error / FULL_SPEED_ABOVE * 100), 0, 100);
  ledcWrite(VTL_PIN_LED_RED, speed * 255 / 100);
  Serial.printf("T=%.1f target=%.1f relay=%s speed=%d%%\n", t, target, fanOn ? "ON" : "off", speed);
}
