/*
  VTLabs IoT Training Kit - L1_08 Traffic Light State Machine
  ------------------------------------------------------------------
  Purpose:     Program a sequence as a state machine with millis(), so the
               board can still react to a pedestrian button.
  Hardware:    LED_RED, LED_YELLOW, LED_GREEN, BTN1 (pedestrian request), BUZZER.
  Libraries:   none
  Expected:    GREEN 6 s -> YELLOW 2 s -> RED 6 s -> repeat.
               Pressing BTN1 during GREEN shortens it to 1 s and the buzzer
               ticks during RED ("safe to cross").
  Experiments: 1. Add a RED+YELLOW state (as used in some countries).
               2. Use SWITCH to select "night mode": blinking yellow only.
  Troubleshooting:
    - Lights do not change: check that loop() contains no long delay().
  @requires: none
*/
#include <VTLabsKit.h>

enum State { GREEN, YELLOW, RED };
State state = GREEN;
unsigned long stateStart = 0, stateTime = 6000, lastTick = 0;
bool requested = false;
VTLButton pedestrian;

void enter(State s, unsigned long ms) {
  state = s;
  stateStart = millis();
  stateTime = ms;
  digitalWrite(VTL_PIN_LED_GREEN, s == GREEN);
  digitalWrite(VTL_PIN_LED_YELLOW, s == YELLOW);
  digitalWrite(VTL_PIN_LED_RED, s == RED);
  Serial.println(s == GREEN ? "GREEN" : s == YELLOW ? "YELLOW" : "RED");
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  pedestrian.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  enter(GREEN, 6000);
}

void loop() {
  pedestrian.update();
  if (pedestrian.pressed() && state == GREEN && !requested) {
    requested = true;
    Serial.println("Pedestrian request");
    if (millis() - stateStart < stateTime - 1000) stateTime = millis() - stateStart + 1000;
  }
  if (state == RED && requested && millis() - lastTick >= 500) {  // crossing signal
    lastTick = millis();
    tone(VTL_PIN_BUZZER, 2500, 30);
  }
  if (millis() - stateStart >= stateTime) {
    if (state == GREEN) enter(YELLOW, 2000);
    else if (state == YELLOW) enter(RED, 6000);
    else { requested = false; enter(GREEN, 6000); }
  }
}
