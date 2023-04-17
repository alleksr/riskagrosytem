#include <SPI.h>
#include <LoRaWAN.h>
#include <Crypto.h>
#include <PubSubClient.h>

// Настройки для TTN (The Things Network)
static const uint8_t PROGMEM DEVEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t PROGMEM APPKEY[16]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Настройки для LoRaWAN
const uint8_t RX_TIMEOUT = 5; // Время ожидания приема данных в секундах
uint8_t buffer[256]; // Буфер для принятых данных

// Настройки для шифрования
byte key[CryptoAES::BLOCKSIZE];
byte iv[CryptoAES::BLOCKSIZE];

void setup() {
  // Настройка модуля sx1262
  SPI.begin(5, 19, 27, 18);
  LoRaWAN.init();
  LoRaWAN.setDevEUI(DEVEUI);
  LoRaWAN.setAppEUI(APPEUI);
  LoRaWAN.setAppKey(APPKEY);
  LoRaWAN.setSubBand(2);
  LoRaWAN.setRxTimeout(RX_TIMEOUT);
  LoRaWAN.join();

  // Генерация ключа и вектора инициализации для шифрования
  // TODO: заменить на настоящие значения
  memset(key, 0, CryptoAES::BLOCKSIZE);
  memset(iv, 0, CryptoAES::BLOCKSIZE);
}

// Настройки MQTT-брокера
const char* mqttServer = "mqtt.server.com";
const int mqttPort = 1883;
const char* mqttUsername = "mqtt_username";
const char* mqttPassword = "mqtt_password";
const char* mqttClientId = "your_client_id";

// Создание клиента MQTT
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
mqttClient.setServer(mqttServer, mqttPort);
mqttClient.setCredentials(mqttUsername, mqttPassword);


void loop() {
  if (LoRaWAN.joined()) {
  int packetSize = LoRaWAN.receive(buffer, sizeof(buffer));
  if (packetSize > 0) {
    // Расшифровка данных
    byte iv[CryptoAES::BLOCKSIZE];
    memset(iv, 0, CryptoAES::BLOCKSIZE);
    byte key[CryptoAES::BLOCKSIZE];
    memcpy(key, APPKEY, CryptoAES::BLOCKSIZE);
    CryptoAES aes(key, CryptoAES::BLOCKSIZE);
    aes.decrypt(buffer + 4, packetSize - 4, iv);
    // Обработка принятых данных
    uint32_t devaddr = LoRaWAN.getDevAddr(buffer);
    for (int i = 0; i < packetSize - 4; i++) {
      Serial.print("Device ");
      Serial.print(devaddr, HEX);
      Serial.print(": ");
      Serial.print((char)buffer[i + 4]);
      Serial.println();

      // Отправка данных в MQTT
      char mqttTopic[50];
      sprintf(mqttTopic, "device/%08X/data", devaddr);
      char mqttPayload[256];
      sprintf(mqttPayload, "%c", buffer[i + 4]);
      mqttClient.publish(mqttTopic, mqttPayload);
    }
  }
} else {
  LoRaWAN.join();
}

}
