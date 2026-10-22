/*
  VTLabs IoT Training Kit - L3_03 Serial Command Parser
  ------------------------------------------------------------------
  Purpose:     Control the kit by typing commands: read a line, split it into
               words, validate, act, and answer OK/ERROR.
  Hardware:    LEDs, RELAY, BUZZER, POT, LDR (on board).
  Libraries:   none
  Expected:    In the Serial Monitor (115200, "Newline" line ending) type:
                 help | led red on | led green off | relay on | beep 1000 |
                 read pot | read ldr | status
  Experiments: 1. Add "blink yellow 5" (blink 5 times).
               2. Accept upper- and lower-case commands.
               3. Make the relay command refuse to switch more than once per second.
  Troubleshooting:
    - "ERROR unknown command": check spelling and the line-ending setting.
  @requires: none
*/
#include <VTLabsKit.h>

String line;

int ledPin(const String &name) {
  if (name == "red") return VTL_PIN_LED_RED;
  if (name == "green") return VTL_PIN_LED_GREEN;
  if (name == "yellow") return VTL_PIN_LED_YELLOW;
  return -1;
}

void handle(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  String w[4];
  int n = 0;
  while (cmd.length() && n < 4) {  // split into words
    int sp = cmd.indexOf(' ');
    w[n++] = sp < 0 ? cmd : cmd.substring(0, sp);
    cmd = sp < 0 ? "" : cmd.substring(sp + 1);
    cmd.trim();
  }
  if (n == 0) return;
  if (w[0] == "help") {
    Serial.println("led <red|green|yellow> <on|off>, relay <on|off>, beep <hz>, read <pot|ldr>, status");
  } else if (w[0] == "led" && n == 3 && ledPin(w[1]) >= 0 && (w[2] == "on" || w[2] == "off")) {
    digitalWrite(ledPin(w[1]), w[2] == "on");
    Serial.println("OK");
  } else if (w[0] == "relay" && n == 2 && (w[1] == "on" || w[1] == "off")) {
    vtlRelayWrite(w[1] == "on");
    Serial.println("OK");
  } else if (w[0] == "beep" && n == 2 && w[1].toInt() >= 100 && w[1].toInt() <= 5000) {
    tone(VTL_PIN_BUZZER, w[1].toInt(), 200);
    Serial.println("OK");
  } else if (w[0] == "read" && n == 2 && (w[1] == "pot" || w[1] == "ldr")) {
    Serial.printf("OK %d\n", analogRead(w[1] == "pot" ? VTL_PIN_POT : VTL_PIN_LDR));
  } else if (w[0] == "status") {
    vtlPrintBoardInfo(Serial);
  } else {
    Serial.println("ERROR unknown command - type help");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_RELAY, OUTPUT);
  vtlRelayWrite(false);
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  pinMode(VTL_PIN_LED_YELLOW, OUTPUT);
  Serial.println("Ready. Type help");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (line.length()) handle(line);
      line = "";
    } else if (line.length() < 60) {
      line += c;
    }
  }
}
