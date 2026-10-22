/*
  VTLabs IoT Training Kit - L3_09 Introduction to RS485 (OPTIONAL MODULE)
  ------------------------------------------------------------------
  Purpose:     RS485 sends UART data as a differential pair (A/B) - up to
               ~1200 m and many devices on one cable. Only ONE device may
               transmit at a time, so the transceiver has a direction pin.
  Hardware:    OPTIONAL TTL-to-RS485 module (3.3 V type, e.g. MAX3485) on Serial2:
               DI <- VTL_PIN_UART2_TX, RO -> VTL_PIN_UART2_RX,
               DE+RE -> VTL_PIN_RS485_DE (or an auto-direction module: DE = -1).
               Two kits: A-A, B-B, GND-GND, 120 ohm terminators at both cable ends.
  Libraries:   none
  Expected:    SWITCH ON  = "master": sends "T?" every 2 s and prints the reply.
               SWITCH OFF = "slave":  answers "T=<pot value>" to each "T?".
  Experiments: 1. Use 3 kits with addresses (e.g. "1:T?").
               2. Measure A and B with a multimeter while idle.
               3. Read about Modbus RTU - the industrial protocol on RS485.
  Troubleshooting:
    - No reply: swap A/B on one side; check both use the same baud rate.
    - Only works one way: the DE pin is not switching (or wrong module type).
  @requires: none
*/
#include <VTLabsKit.h>

const long BAUD = 9600;
String rx;

void txMode(bool on) {
#if VTL_PIN_RS485_DE >= 0
  digitalWrite(VTL_PIN_RS485_DE, on ? HIGH : LOW);
#else
  (void)on;  // auto-direction module
#endif
}

void sendLine(const String &s) {
  txMode(true);
  Serial2.println(s);
  Serial2.flush();  // wait until the last bit has left the UART
  txMode(false);
}

void setup() {
  Serial.begin(115200);
#if VTL_PIN_RS485_DE >= 0
  pinMode(VTL_PIN_RS485_DE, OUTPUT);
#endif
  txMode(false);
  pinMode(VTL_PIN_SWITCH, VTL_PIN_SWITCH_MODE);
  Serial2.begin(BAUD, SERIAL_8N1, VTL_PIN_UART2_RX, VTL_PIN_UART2_TX);
}

void loop() {
  bool master = digitalRead(VTL_PIN_SWITCH) == VTL_ACTIVE_LEVEL(SWITCH);
  static unsigned long last = 0;
  if (master && millis() - last > 2000) {
    last = millis();
    sendLine("T?");
  }
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c != '\n') { if (c != '\r' && rx.length() < 40) rx += c; continue; }
    Serial.printf("[%s] received: %s\n", master ? "master" : "slave", rx.c_str());
    if (!master && rx == "T?") sendLine("T=" + String(analogRead(VTL_PIN_POT)));
    rx = "";
  }
}
