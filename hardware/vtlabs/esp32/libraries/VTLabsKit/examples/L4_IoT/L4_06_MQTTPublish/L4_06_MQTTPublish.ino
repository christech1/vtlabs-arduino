/*
  VTLabs IoT Training Kit - L4_06 MQTT: Publish and Subscribe (advanced)
  ------------------------------------------------------------------
  Purpose:     Send sensor data to an MQTT broker and receive commands.
               MQTT is the publish/subscribe protocol used by most IoT platforms.
  Hardware:    DHT22, LDR, LED_GREEN (on board). Internet or a local broker.
  Setup:       1. Wi-Fi details in the "arduino_secrets.h" tab.
               2. MQTT_HOST: a classroom broker (e.g. Mosquitto on a laptop) or
                  the public test broker test.mosquitto.org (no privacy - test only!).
  Libraries:   "PubSubClient" by Nick O'Leary (>= 2.8),
               "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    Every 10 s publishes JSON to  vtlabs/<chip-id>/telemetry
               Subscribes to vtlabs/<chip-id>/led  ("on"/"off" -> LED_GREEN)
               Test from a laptop:
                 mosquitto_sub -h <broker> -t "vtlabs/#" -v
                 mosquitto_pub -h <broker> -t vtlabs/<chip-id>/led -m on
  Experiments: 1. Publish an "online"/"offline" status using MQTT Last Will.
               2. Add a topic to set the publish interval.
               3. Show the data in a dashboard tool (Node-RED, Home Assistant...).
  Troubleshooting:
    - "MQTT connect failed, rc=-2": broker address/port wrong or blocked network.
    - Messages longer than 256 bytes are dropped: client.setBufferSize(512).
  @requires: none
*/
#include <VTLabsKit.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "arduino_secrets.h"

const char *MQTT_HOST = "test.mosquitto.org";
const uint16_t MQTT_PORT = 1883;

WiFiClient net;
PubSubClient mqtt(net);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
String id, topicTelemetry, topicLed;

void onMessage(char *topic, byte *payload, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  Serial.printf("received %s = %s\n", topic, msg.c_str());
  if (String(topic) == topicLed) digitalWrite(VTL_PIN_LED_GREEN, msg == "on");
}

void ensureMqtt() {
  while (!mqtt.connected()) {
    Serial.print("MQTT connecting... ");
    if (mqtt.connect(id.c_str())) {
      Serial.println("ok");
      mqtt.subscribe(topicLed.c_str());
    } else {
      Serial.printf("failed, rc=%d - retry in 5 s\n", mqtt.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  dht.begin();
  id = "vtlabs-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  topicTelemetry = "vtlabs/" + id + "/telemetry";
  topicLed = "vtlabs/" + id + "/led";

  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.printf("\nWi-Fi ok. Device id: %s\n", id.c_str());
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
}

void loop() {
  ensureMqtt();
  mqtt.loop();
  static unsigned long last = 0;
  if (millis() - last >= 10000) {
    last = millis();
    String json = "{\"t\":" + String(dht.readTemperature(), 1) + ",\"h\":" + String(dht.readHumidity(), 1) +
                  ",\"light\":" + String(map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100)) + "}";
    mqtt.publish(topicTelemetry.c_str(), json.c_str());
    Serial.println("published " + json);
  }
}
