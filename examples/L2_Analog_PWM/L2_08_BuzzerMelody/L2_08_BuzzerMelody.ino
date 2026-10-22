/*
  VTLabs IoT Training Kit - L2_08 Tones and a Melody
  ------------------------------------------------------------------
  Purpose:     A passive buzzer plays the frequency you send with tone().
               Frequency sets the pitch; PWM duty barely changes loudness.
  Hardware:    BUZZER (passive, on board), POT, BTN1.
  Libraries:   none
  Expected:    BTN1 plays a short melody. Otherwise the POT works as a
               "theremin": its position sets a tone from 200 to 2000 Hz
               while SWITCH is ON.
  Experiments: 1. Write your own melody in the notes[] table.
               2. Make a siren that sweeps 600 -> 1200 Hz and back.
               3. What happens with an ACTIVE buzzer (built-in oscillator)?
  Troubleshooting:
    - Only clicks: buzzer is active-type or the driver transistor is missing.
  @requires: none
*/
#include <VTLabsKit.h>

const int notes[] = {262, 294, 330, 349, 392, 440, 494, 523};  // C4..C5
VTLButton btn;

void setup() {
  pinMode(VTL_PIN_SWITCH, VTL_PIN_SWITCH_MODE);
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
}

void loop() {
  btn.update();
  if (btn.pressed()) {
    for (int f : notes) {
      tone(VTL_PIN_BUZZER, f, 180);
      delay(220);
    }
  } else if (digitalRead(VTL_PIN_SWITCH) == VTL_ACTIVE_LEVEL(SWITCH)) {
    int f = map(analogRead(VTL_PIN_POT), 0, 4095, 200, 2000);
    tone(VTL_PIN_BUZZER, f);
    delay(30);
  } else {
    noTone(VTL_PIN_BUZZER);
  }
}
