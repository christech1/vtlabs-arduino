/*
  Test: a sketch can override one pin without touching the board package.
  @requires: none
*/
#define VTL_PIN_LED_RED 13
#include <VTLabsKit.h>
static_assert(VTL_PIN_LED_RED == 13, "per-sketch override must win");
static_assert(VTL_PIN_LED_GREEN == 26, "other pins must keep the profile value");
void setup() {
  pinMode(VTL_PIN_LED_RED, OUTPUT);
}
void loop() {
}
