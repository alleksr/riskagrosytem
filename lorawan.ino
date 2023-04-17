#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <AsyncMqttClient.h>

// Replace with your TTN keys
const char *devEui = "INSERT_DEV_EUI_HERE";
const char *appEui = "INSERT_APP_EUI_HERE";
const char *appKey = "INSERT_APP_KEY_HERE";

// Replace with your WiFi credentials
const char *ssid = "INSERT_WIFI_SSID_HERE";
const char *password = "INSERT_WIFI_PASSWORD_HERE";

// Replace with your MQTT broker IP address
const char *mqttServer = "INSERT_MQTT_BROKER_IP_HERE";
const uint16_t mqttPort = 1883;

// Replace with your MQTT credentials
const char *mqttUser = "INSERT_MQTT_USER_HERE";
const char *mqttPassword = "INSERT_MQTT_PASSWORD_HERE";

// MQTT topics to publish to
const char *mqttTopic1 = "mytopic1";
const char *mqttTopic2 = "mytopic2";

// Pin mapping for LoRa module
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,
  .dio = {2, 6, 7},
};

// Global variables for LoRaWAN
static osjob_t sendjob;
const uint8_t maxPayloadLength = 51;
uint8_t payload[maxPayloadLength];
uint8_t payloadSize = 0;

// Create an instance of the AsyncMqttClient
AsyncMqttClient mqttClient;

// WiFi event handler
void onWiFiConnect(WiFiEvent_t event) {
  Serial.println("Connected to Wi-Fi.");
}

void onWiFiDisconnect(WiFiEvent_t event) {
  Serial.println("Disconnected from Wi-Fi.");
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT broker.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  // Subscribe to the MQTT topics
  mqttClient.subscribe(mqttTopic1, 0);
  mqttClient.subscribe(mqttTopic2, 0);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT broker.");
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Message received from MQTT broker:");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.write((uint8_t*)payload, len);
  Serial.println();
}

void do_send(osjob_t *j) {
  // Create a buffer to store the LoRaWAN message
  uint8_t buffer[maxPayloadLength];
  memcpy(buffer, payload, payloadSize);
  
  // Prepare the LoRaWAN message
  LMIC.frame[0] = 0x00;
  LMIC.frame[1] = 0x00;
  LMIC.frame[2] = 0x00;
  LMIC.frame[3] = 0x00;
  LMIC_setTxData2(1, buffer, payloadSize, 0);

  Serial.println("Packet queued for transmission.");
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("
void sendData() {
  if (!LoRa.available()) {
    return;
  }

  // Получаем данные от LoRa
  String payload = "";

  while (LoRa.available()) {
    payload += (char)LoRa.read();
  }

  // Шифруем данные
  byte key[16];
  byte iv[16];
  generateKeyAndIV(key, iv);
  String encryptedPayload = encryptAES(payload, key, iv);

  // Отправляем данные на MQTT брокер
  if (mqtt.connected()) {
    AsyncMqttClientMessageProperties properties;
    properties.qos = 1;
    properties.retain = false;
    mqtt.publish("sensors/risk-agrosystem", 1, true, encryptedPayload.c_str(), encryptedPayload.length(), properties);
  }
}
