/*
  VTLabs IoT Training Kit - L1_03 Button Controls an LED
  ------------------------------------------------------------------
  Purpose:     Combine an input and an output: hold-to-light, then toggle.
  Hardware:    BTN1, LED_RED, LED_GREEN (on board).
  Libraries:   none
  Expected:    Red LED is on while BTN1 is held.
               Green LED toggles each time BTN1 is pressed (edge detection).
  Experiments: 1. Use BTN2 to toggle the yellow LED.
               2. Make the green LED toggle on release instead of press.
               3. Press very quickly many times: does the toggle sometimes
                  miss or double? (This is contact bounce - see L1_05.)
  Troubleshooting:
    - Green LED toggles twice per press: bounce; lesson L1_05 fixes it.
  @requires: none
*/
#include <VTLabsKit.h>

bool greenOn = false;
bool wasPressed = false;

void setup() {
  pinMode(VTL_PIN_BTN1, VTL_PIN_BTN1_MODE);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
}

void loop() {
  bool pressed = digitalRead(VTL_PIN_BTN1) == VTL_ACTIVE_LEVEL(BTN1);

  // 1) level: LED follows the button
  digitalWrite(VTL_PIN_LED_RED, pressed ? HIGH : LOW);

  // 2) edge: toggle only at the moment the button goes from released to pressed
  if (pressed && !wasPressed) {
    greenOn = !greenOn;
    digitalWrite(VTL_PIN_LED_GREEN, greenOn ? HIGH : LOW);
  }
  wasPressed = pressed;
  delay(10);
}
