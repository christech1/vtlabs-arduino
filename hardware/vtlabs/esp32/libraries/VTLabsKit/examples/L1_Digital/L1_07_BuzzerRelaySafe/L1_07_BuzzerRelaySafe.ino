/*
  VTLabs IoT Training Kit - L1_07 Buzzer and Relay - Safely
  ------------------------------------------------------------------
  Purpose:     Drive actuators through their transistor drivers, with a
               known safe state at start-up and an automatic switch-off.
  Hardware:    BUZZER (passive, via transistor), RELAY (via transistor and
               flyback diode), BTN1, LED_RED.
  SAFETY:      Use the relay ONLY with safe low-voltage loads (e.g. 12 V lamp
               or the on-board LED test). Never connect mains in class.
  Libraries:   none
  Expected:    BTN1 press -> short beep, relay ON for 5 s (red LED shows
               relay state), then relay OFF automatically with a double beep.
  Experiments: 1. Change ON_TIME_MS. 2. Press BTN1 while ON to switch off early.
               3. Between reset and setup() the GPIO floats. What keeps the relay
                  OFF during that time? (Hint: the driver's pull-down resistor.)
  Troubleshooting:
    - Relay clicks at power-up: the GPIO floats before setup(); the board
      should have a pull-down on the driver input.
    - Buzzer silent: passive buzzers need a tone (PWM), not just HIGH.
  @requires: none
*/
#include <VTLabsKit.h>

const unsigned long ON_TIME_MS = 5000;
bool relayOn = false;
unsigned long relayStart = 0;
VTLButton button;

void setRelay(bool on) {
  relayOn = on;
  vtlRelayWrite(on);                      // honours the board's relay polarity
  digitalWrite(VTL_PIN_LED_RED, on);
  Serial.println(on ? "Relay ON" : "Relay OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_RELAY, OUTPUT);         // on ESP32 core 3.x a pin must be an output
  vtlRelayWrite(false);                   // before digitalWrite() works - so set OFF immediately
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  button.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  setRelay(false);
}

void loop() {
  button.update();
  if (button.pressed()) {
    if (!relayOn) {
      tone(VTL_PIN_BUZZER, 2000, 100);
      setRelay(true);
      relayStart = millis();
    } else {
      setRelay(false);                    // manual early switch-off
    }
  }
  if (relayOn && millis() - relayStart >= ON_TIME_MS) {
    setRelay(false);
    tone(VTL_PIN_BUZZER, 1500, 80);
    delay(150);
    tone(VTL_PIN_BUZZER, 1500, 80);
  }
}
