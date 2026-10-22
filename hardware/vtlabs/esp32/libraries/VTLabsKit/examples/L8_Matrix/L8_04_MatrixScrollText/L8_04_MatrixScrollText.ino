/*
  VTLabs IoT Training Kit - L8_04 Scrolling Text (any matrix)
  ------------------------------------------------------------------
  Purpose:     Scroll text WITHOUT blocking the program: scrollText() starts,
               scrollUpdate() moves one step when it is time and returns true
               when the text has left the display. Meanwhile loop() keeps
               reading buttons - compare with a version using delay().
  Hardware:    LED matrix add-on (Tools > LED Matrix (add-on)), POT (speed), BTN1.
  Libraries:   as L8_01
  Expected:    "Welcome to VTLabs" scrolls repeatedly. The POT changes the speed.
               BTN1 switches to a second message immediately (no waiting).
               Type in the Serial Monitor to scroll your own text.
  Experiments: 1. Show the time since start (uptime) as scrolling text.
               2. Change colour on every pass (WS2812 only).
               3. Explain why a delay()-based scroll would make BTN1 feel "dead".
  Troubleshooting:
    - Text cut off: messages are limited to 95 characters.
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>

VTLMatrix matrix;
VTLButton btn;
String messages[2] = {"Welcome to VTLabs", "IoT Training Kit - ESP32"};
int current = 0;

uint16_t speedMs() { return map(analogRead(VTL_PIN_POT), 0, 4095, 20, 150); }

void setup() {
  Serial.begin(115200);
  matrix.begin();
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  matrix.scrollText(messages[current].c_str(), speedMs());
}

void loop() {
  btn.update();
  if (btn.pressed()) {
    current = 1 - current;
    matrix.scrollText(messages[current].c_str(), speedMs());  // restart immediately
  }
  if (Serial.available()) {
    messages[current] = Serial.readStringUntil('\n');
    messages[current].trim();
    matrix.scrollText(messages[current].c_str(), speedMs());
  }
  if (matrix.scrollUpdate()) {  // one pass finished - start the next one
    matrix.setColor(current ? VTL_ORANGE : VTL_GREEN);
    matrix.scrollText(messages[current].c_str(), speedMs());
  }
}
