// Ziyi Wang Oct 2025 - v1 - MQTT messager to vespera with temperature and touch sensors
// Works with MKR1010

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>

#define TEMP_PIN A0   // Temperature sensor

//Network Configuration
const char* ssid          = "Glide-US-Resident";
const char* password      = "ColdAgedCadet";
const char* mqtt_username = "student";
const char* mqtt_password = "ce2021-mqtt-forget-whale";
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

// create wifi object and mqtt object
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Make sure to update your lightid value below with the one you have been allocated
String lightId = "27"; // the topic id number or user number being used.

// Here we define the MQTT topic we will be publishing data to
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;            
String clientId = ""; // will set once i have mac address so that it is unique

// NeoPixel Configuration - we need to know this to know how to send messages 
// to vespera 
const int num_leds = 72;
const int payload_size = num_leds * 3; // x3 for RGB

// Create the byte array to send in MQTT payload this stores all the colours 
// in memory so that they can be accessed in for example the rainbow function
byte RGBpayload[payload_size];

//Function declaration
void printMacAddress(byte mac[]);
void startWifi();
void reconnectMQTT();
void callback(char* topic, byte* payload, unsigned int length);

//Setup
void setup() {
  Serial.begin(115200);
  //while (!Serial); // Wait for serial port to connect (useful for debugging)
  Serial.println("Vespera");

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);

  // Connect to WiFi
  startWifi();

  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(2000);
  mqttClient.setCallback(callback);

  Serial.println("Set-up complete");
}

//////////////////////Main Loop///////////////////////
void loop() {
  // Reconnect if necessary
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  if (WiFi.status() != WL_CONNECTED){
    startWifi();
  }
  // keep mqtt alive
  mqttClient.loop();

  int raw = analogRead(TEMP_PIN);               // Read T
  float voltage = raw * (3.3 / 1023.0);         // Calculate V
  float temperatureC = (voltage - 0.5) / 0.01;  // Calculate ℃, 1℃ for every 0.01V

  // Print T in monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // Map T to colour
  int r = constrain(map(temperatureC, 20, 35, 0, 255), 0, 255); // T ⬆, r ⬆
  int b = constrain(map(temperatureC, 20, 35, 255, 0), 0, 255); // T ⬇, b ⬆
  int g = 50;

  for (int n = 0; n < num_leds; n++) {
    RGBpayload[n * 3 + 0] = (byte)r;
    RGBpayload[n * 3 + 1] = (byte)g;
    RGBpayload[n * 3 + 2] = (byte)b;
  }

  if (mqttClient.connected()) {
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
    Serial.print("Published color → R:");
    Serial.print(r);
    Serial.print(" G:");
    Serial.print(g);
    Serial.print(" B:");
    Serial.println(b);
  }

  delay(1000);
}


void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i > 0) Serial.print(":");
  }
  Serial.println();
}

