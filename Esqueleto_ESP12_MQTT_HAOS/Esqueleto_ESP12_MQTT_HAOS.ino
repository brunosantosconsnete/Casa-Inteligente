#include <Wire.h>
#include <ArduinoHA.h>
#include "ESP8266WiFi.h"
#define BROKER_ADDR IPAddress(192,168,16,1)
#define BROKER_USERNAME  "vitor" // replace with your credentials
#define BROKER_PASSWORD  "C0n$n3t3"

HADevice device;
WiFiClient espClient;
HAMqtt mqtt(espClient, device);
const char* ssid = "consnete";
const char* senha = "cons1490";
HASensorNumber SCT1("Fase1");
HASensorNumber SCT2("Fase2");
IPAddress local_IP(192, 168, 16, 155);
IPAddress gateway(192, 168, 16, 1);
IPAddress subnet(255, 255, 0, 0);

void WF_Begin() {
if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
byte mac[WL_MAC_ADDR_LENGTH];
WiFi.macAddress(mac);
device.setUniqueId(mac, sizeof(mac));
WiFi.begin(ssid, senha);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
Serial.println("");
Serial.println("WiFi Conectada");
Serial.println(WiFi.localIP());
}

void MQTT_Begin() {
mqtt.onConnected(onMqttConnected);
mqtt.onDisconnected(onMqttDisconnected);
mqtt.onStateChanged(onMqttStateChanged);
mqtt.begin(BROKER_ADDR, BROKER_USERNAME, BROKER_PASSWORD);
}

void onMqttConnected() {
    Serial.println("Connected to the broker!");

    // You can subscribe to custom topic if you need
    mqtt.subscribe("Teste");
}

void onMqttDisconnected() {
    Serial.println("Disconnected from the broker!");
}

void onMqttStateChanged(HAMqtt::ConnectionState state) {
    Serial.print("MQTT state changed to: ");
    Serial.println(static_cast<int8_t>(state));
}

void HAOS_setup() {
device.setName("Central Bruno");
device.setSoftwareVersion("1.0.0");
SCT1.setIcon("mdi:home-lightning-bolt");
SCT1.setName("Fase 1");
SCT1.setUnitOfMeasurement("W");
SCT2.setIcon("mdi:home-lightning-bolt");
SCT2.setName("Fase 2");
SCT2.setUnitOfMeasurement("W");
}

void setup() {
Serial.begin(115200);
WF_Begin();
HAOS_setup();
MQTT_Begin();
delay(100);
Serial.println(F("Init....")); 
}

void loop() {
 mqtt.loop();
 long timer = 0;

 SCT1.setValue(0);
 SCT2.setValue(0);

}
