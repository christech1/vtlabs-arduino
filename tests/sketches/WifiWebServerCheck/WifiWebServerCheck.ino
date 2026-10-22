#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);
// @requires: none
void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("VTLabs-Test", "vtlabs1234");
  server.on("/", []() { server.send(200, "text/plain", "VTLabs OK"); });
  server.begin();
}
void loop() {
  server.handleClient();
}
