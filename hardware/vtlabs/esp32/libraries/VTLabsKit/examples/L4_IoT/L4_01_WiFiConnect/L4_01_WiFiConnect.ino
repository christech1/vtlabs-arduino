/*
  VTLabs IoT Training Kit - L4_01 Connect to Wi-Fi
  ------------------------------------------------------------------
  Purpose:     Join a Wi-Fi network as a station, with a timeout, status
               LED, signal strength and automatic reconnection.
  Hardware:    Kit only. A 2.4 GHz Wi-Fi network (ESP32 cannot use 5 GHz).
  Setup:       Open the "arduino_secrets.h" tab and enter your network name/password.
  Libraries:   none (WiFi is part of the ESP32 core)
  Expected:    Blue LED blinks while connecting, stays on when connected.
               Serial shows IP address, RSSI (signal, dBm) and MAC address.
               Unplug the router: the sketch notices and reconnects.
  Experiments: 1. Walk away from the router and watch RSSI. At what value does it drop?
               2. Print WiFi.gatewayIP() and WiFi.dnsIP().
               3. Scan for networks with WiFi.scanNetworks() and list them.
  Troubleshooting:
    - Status 1 (NO_SSID_AVAIL): wrong name or a 5 GHz-only network.
    - Status 4 (CONNECT_FAILED) / never connects: wrong password.
    - Analog readings stop working on some pins: ADC2 is blocked by Wi-Fi.
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include "arduino_secrets.h"

const unsigned long CONNECT_TIMEOUT_MS = 20000;

bool connectWiFi() {
  Serial.printf("Connecting to \"%s\"", SECRET_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < CONNECT_TIMEOUT_MS) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.print('.');
    delay(250);
  }
  Serial.println();
  bool ok = WiFi.status() == WL_CONNECTED;
  digitalWrite(LED_BUILTIN, ok);
  if (ok) {
    Serial.printf("Connected. IP %s  RSSI %d dBm  MAC %s\n", WiFi.localIP().toString().c_str(), WiFi.RSSI(),
                  WiFi.macAddress().c_str());
  } else {
    Serial.printf("Failed, status %d\n", WiFi.status());
  }
  return ok;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  connectWiFi();
}

void loop() {
  static unsigned long last = 0;
  if (millis() - last < 5000) return;
  last = millis();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi lost - reconnecting");
    WiFi.disconnect();
    connectWiFi();
  } else {
    Serial.printf("still connected, RSSI %d dBm\n", WiFi.RSSI());
  }
}
