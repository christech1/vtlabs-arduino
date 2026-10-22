/*
  VTLabs IoT Training Kit - L3_02 Exchanging Data Between Two UART Devices
  ------------------------------------------------------------------
  Purpose:     Use the second hardware UART (Serial2) to talk to another device.
  Hardware:    Option A (one kit):  jumper wire from UART2 TX to UART2 RX (loopback).
               Option B (two kits): kit1 TX2 -> kit2 RX2, kit1 RX2 <- kit2 TX2, GND <-> GND.
               Pins: see VTL_PIN_UART2_TX / VTL_PIN_UART2_RX in docs/pinmaps/.
  Libraries:   none
  Expected:    Every 2 s the board sends "PING <n> pot=<value>".
               Anything received on Serial2 is printed on the USB Serial Monitor.
               Text you type in the Serial Monitor is forwarded to Serial2.
               Received "LED ON"/"LED OFF" switches LED_GREEN.
  Experiments: 1. Send "LED ON" from the other kit.  2. Change the baud rate on
               one side only - what is received?  3. Add a checksum to each line.
  Troubleshooting:
    - Nothing received: TX must go to RX (crossed), and GND must be shared.
    - Both kits MUST be 3.3 V logic - never connect 5 V UART signals directly.
  @requires: none
*/
#include <VTLabsKit.h>

const long UART_BAUD = 9600;
unsigned long counter = 0, lastPing = 0;
String rxLine;

void setup() {
  Serial.begin(115200);
  Serial2.begin(UART_BAUD, SERIAL_8N1, VTL_PIN_UART2_RX, VTL_PIN_UART2_TX);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  Serial.println("UART2 ready. Type text to send it over Serial2.");
}

void loop() {
  if (millis() - lastPing >= 2000) {
    lastPing = millis();
    Serial2.printf("PING %lu pot=%d\n", ++counter, analogRead(VTL_PIN_POT));
  }
  while (Serial.available()) Serial2.write(Serial.read());  // USB -> UART2

  while (Serial2.available()) {                              // UART2 -> USB
    char c = Serial2.read();
    if (c == '\n') {
      Serial.print("RX2: ");
      Serial.println(rxLine);
      rxLine.trim();
      if (rxLine == "LED ON") digitalWrite(VTL_PIN_LED_GREEN, HIGH);
      if (rxLine == "LED OFF") digitalWrite(VTL_PIN_LED_GREEN, LOW);
      rxLine = "";
    } else if (c != '\r' && rxLine.length() < 80) {
      rxLine += c;
    }
  }
}
