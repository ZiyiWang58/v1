// Ziyi Wang Oct 2025 - v3 - MQTT messager to vespera with temperature and touch sensors (change states)
// Works with MKR1010

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>

#define TEMP_PIN A0         // Temperature sensor
#define TOUCH_PIN A2        // Touch sensor

int mode = 0;               // 0 = before first detection, 1 = fix first detection before second detection, 2 = fix both
bool lastTouchState = LOW;  // Detect touch

// Network Configuration
const char* ssid          = "Glide-US-Resident";
const char* password      = "ColdAgedCadet";
const char* mqtt_username = "student";
const char* mqtt_password = "ce2021-mqtt-forget-whale";
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;

// create wifi object and mqtt object
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Make sure to update your lightid value below with the one you have been allocated
String lightId = "27";  // the topic id number or user number being used.

// Here we define the MQTT topic we will be publishing data to
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;
String clientId = "";  // will set once i have mac address so that it is unique

// NeoPixel Configuration - we need to know this to know how to send messages
// NeoPixel Configuration
const int num_leds = 72;
const int payload_size = num_leds * 3;  // x3 for RGB

// Create the byte array to send in MQTT payload this stores all the colours
// in memory so that they can be accessed in for example the rainbow function
byte RGBpayload[payload_size];

// Function declaration
void printMacAddress(byte mac[]);
void startWifi();
void reconnectMQTT();
void callback(char* topic, byte* payload, unsigned int length);

// Setup
void setup() {
  Serial.begin(115200);
  //while (!Serial); // Wait for serial port to connect (useful for debugging)
  Serial.println("Vespera");

  pinMode(TOUCH_PIN, INPUT);

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

  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // Keep mqtt alive
  mqttClient.loop();

  // Read touch sensor
  int touchState = digitalRead(TOUCH_PIN);
  if (touchState == HIGH && lastTouchState == LOW) {
    mode++;
    if (mode > 2) mode = 0;
    Serial.print("Touch detected. Mode: ");
    Serial.println(mode);
    delay(300);
  }
  lastTouchState = touchState;

  // Read temperature
  int raw = analogRead(TEMP_PIN);
  float voltage = raw * (3.3 / 1023.0);
  float temperatureC = (voltage - 0.5) / 0.01;

  // Map temperature to colour
  int r = constrain(map(temperatureC, 20, 35, 0, 255), 0, 255);
  int b = constrain(map(temperatureC, 20, 35, 255, 0), 0, 255);
  int g = 50;

  // Mode 0: Only first half shows colour, second half off
  if (mode == 0) {
    for (int n = 0; n < num_leds; n++) {
      if (n < num_leds / 2) {
        RGBpayload[n * 3 + 0] = (byte)r;
        RGBpayload[n * 3 + 1] = (byte)g;
        RGBpayload[n * 3 + 2] = (byte)b;
      } else {
        RGBpayload[n * 3 + 0] = 0;
        RGBpayload[n * 3 + 1] = 0;
        RGBpayload[n * 3 + 2] = 0;
      }
    }
  }

  // Mode 1: Fix first half, second half shows new temperature
  else if (mode == 1) {
    static int r_fixed1, g_fixed1, b_fixed1;
    if (lastTouchState == HIGH) {
      r_fixed1 = r; g_fixed1 = g; b_fixed1 = b;
    }
    for (int n = 0; n < num_leds; n++) {
      if (n < num_leds / 2) {
        RGBpayload[n * 3 + 0] = r_fixed1;
        RGBpayload[n * 3 + 1] = g_fixed1;
        RGBpayload[n * 3 + 2] = b_fixed1;
      } else {
        RGBpayload[n * 3 + 0] = (byte)r;
        RGBpayload[n * 3 + 1] = (byte)g;
        RGBpayload[n * 3 + 2] = (byte)b;
      }
    }
  }

  // Mode 2: Fix both halves
  else if (mode == 2) {
    static int r_fixed2, g_fixed2, b_fixed2;
    if (lastTouchState == HIGH) {
      r_fixed2 = r; g_fixed2 = g; b_fixed2 = b;
    }
    for (int n = 0; n < num_leds; n++) {
      if (n < num_leds / 2) {
        RGBpayload[n * 3 + 0] = RGBpayload[n * 3 + 0];
      } else {
        RGBpayload[n * 3 + 0] = r_fixed2;
        RGBpayload[n * 3 + 1] = g_fixed2;
        RGBpayload[n * 3 + 2] = b_fixed2;
      }
    }
  }

  // Publish to MQTT
  if (mqttClient.connected()) {
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  }

  // Print to Serial
  Serial.print("Temp: ");
  Serial.print(temperatureC);
  Serial.print(" °C | Mode: ");
  Serial.println(mode);

  delay(100);
}

// Print MAC address
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i > 0) Serial.print(":");
  }
  Serial.println();
}
