/*
  VTLabs IoT Training Kit - L7_02 MAX7219: Text and Effects with MD_Parola
  ------------------------------------------------------------------
  Purpose:     MD_Parola draws fonts and animations on top of MD_MAX72XX.
               displayAnimate() must be called very often - it does one small
               animation step each time and returns true when finished.
  Hardware:    MAX7219 module add-on (see L7_01 for wiring).
               Best on a 4-in-1 (32x8) module; works on 8x8 (one letter at a time).
  Libraries:   "MD_Parola" (>= 3.7) + "MD_MAX72XX" by majicDesigns
  Expected:    Messages appear with different effects: scroll left, wipe,
               opening, scroll up... Type text in the Serial Monitor to show it.
  Experiments: 1. Change SPEED and PAUSE_MS.
               2. Try other effects from the textEffect_t list (PA_SPRITE ...).
               3. Show the potentiometer value, updated each cycle.
  Troubleshooting:
    - Letters mirrored/upside down: wrong MAX7219_HW_TYPE in the board profile.
  @requires: matrix-max7219
*/
#include <VTLabsKit.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

#if !VTL_HAS_MATRIX || VTL_MATRIX_DRIVER != VTL_MATRIX_DRIVER_MAX7219
#error "This lesson is for MAX7219 matrices. Select a MAX7219 module in Tools > LED Matrix (add-on)."
#endif

const uint16_t SPEED = 40, PAUSE_MS = 1500;
MD_Parola parola(VTL_MAX7219_HW_TYPE, SPI, VTL_PIN_MAX7219_CS, VTL_MAX7219_DEVICES);

struct Msg { const char *text; textEffect_t effect; };
Msg msgs[] = {{"VTLabs", PA_SCROLL_LEFT}, {"IoT Kit", PA_WIPE}, {"ESP32", PA_OPENING},
              {"Hello", PA_SCROLL_UP}, {"Rwanda", PA_GROW_UP}};
int idx = 0;
char userText[64] = "";

void setup() {
  Serial.begin(115200);
  SPI.begin(VTL_PIN_SPI_SCK, VTL_PIN_SPI_MISO, VTL_PIN_SPI_MOSI);
  parola.begin();
  parola.setIntensity(3);
  parola.displayText(msgs[0].text, PA_CENTER, SPEED, PAUSE_MS, msgs[0].effect, msgs[0].effect);
  Serial.println("Type a message and press Enter");
}

void loop() {
  if (parola.displayAnimate()) {  // current animation finished
    if (userText[0]) {
      parola.displayText(userText, PA_LEFT, SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
      userText[0] = 0;
    } else {
      idx = (idx + 1) % (sizeof(msgs) / sizeof(msgs[0]));
      parola.displayText(msgs[idx].text, PA_CENTER, SPEED, PAUSE_MS, msgs[idx].effect, msgs[idx].effect);
    }
  }
  static String in;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { strncpy(userText, in.c_str(), sizeof(userText) - 1); in = ""; }
    else if (c != '\r' && in.length() < 60) in += c;
  }
}
