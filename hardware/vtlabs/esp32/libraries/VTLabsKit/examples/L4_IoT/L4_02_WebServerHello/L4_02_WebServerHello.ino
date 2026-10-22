/*
  VTLabs IoT Training Kit - L4_02 Host a Local Web Server
  ------------------------------------------------------------------
  Purpose:     Serve web pages from the ESP32: routes, HTTP status codes,
               query parameters.
  Hardware:    Kit only + phone or laptop on the same Wi-Fi.
  Setup:       Enter Wi-Fi details in the "arduino_secrets.h" tab.
  Libraries:   none (WiFi, WebServer and ESPmDNS are in the ESP32 core)
  Expected:    Serial shows the IP. Open http://<IP>/ or http://vtlabs.local/
                 /            welcome page
                 /hello?name=Ana   "Hello, Ana!"
                 /uptime      seconds since start
                 anything else  404 Not Found
  Experiments: 1. Add /pot that returns the potentiometer value.
               2. Count visitors and show the number on "/".
               3. Open the page from two devices at the same time.
  Troubleshooting:
    - Page does not open: phone must be on the same network (not mobile data).
    - vtlabs.local does not work on some Android phones - use the IP.
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "arduino_secrets.h"

WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.printf("\nOpen http://%s/\n", WiFi.localIP().toString().c_str());
  if (MDNS.begin("vtlabs")) Serial.println("or http://vtlabs.local/");

  server.on("/", []() {
    server.send(200, "text/html",
                "<h1>VTLabs IoT Kit</h1><p>Try <a href='/hello?name=Student'>/hello</a> "
                "or <a href='/uptime'>/uptime</a></p>");
  });
  server.on("/hello", []() {
    String name = server.hasArg("name") ? server.arg("name") : "world";
    server.send(200, "text/plain", "Hello, " + name + "!");
  });
  server.on("/uptime", []() { server.send(200, "text/plain", String(millis() / 1000) + " s"); });
  server.onNotFound([]() { server.send(404, "text/plain", "404 Not Found: " + server.uri()); });
  server.begin();
}

void loop() {
  server.handleClient();
}
