#include <RadioLib.h>
#include <Wire.h>
#include <SHT4x.h>

#define SS_PIN       5    // выберите пин для чип-селекта SX1262
#define RST_PIN     (4)   // выберите пин сброса для SX1262

SX1262 radio = new Module(SS_PIN, RST_PIN);  // создаем экземпляр объекта SX1262

SHT4x sht40;
float temperature, humidity;

const int COMMAND_SIZE = 2;
const int ENCRYPT_SIZE = 16;

const uint8_t encryptionKey[ENCRYPT_SIZE] = {
  0x12, 0xAB, 0x34, 0x56, 0xCD, 0xEF, 0x78, 0x9A,
  0x0B, 0xDE, 0xF1, 0x32, 0x45, 0x67, 0x89, 0xBC
};

bool encryptData = true;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Инициализация SX1262
  int state = radio.beginFSK(868.0, 125.0, 5000);  // Используется режим FSK с частотой 868 МГц, скоростью 125 кБит/с и мощностью 5 дБм
  
  if (state == ERR_NONE) {
    Serial.println("SX1262 initialization OK!");
  } else {
    Serial.print("SX1262 initialization failed with code ");
    Serial.println(state);
    abort();
  }
  
  radio.setPacketType(packetType);
  
  // Инициализация датчика SHT40
  if (!sht40.begin()) {
    Serial.println("SHT40 initialization failed!");
    while (1) {
      delay(1000);
    }
  }

}

String encryptCommand(String command) {
  String encryptedCommand = command;
  
  if (encryptData) {
    aes128_enc_single(encryptionKey, (uint8_t*)encryptedCommand.c_str());
  }
  
  return encryptedCommand;
}

String decryptCommand(String command) {
  String decryptedCommand = command;
  
  if (encryptData) {
    aes128_dec_single(encryptionKey, (uint8_t*)decryptedCommand.c_str());
  }
  
  return decryptedCommand;
}

void sendTemperatureAndHumidity() {
  temperature = sht40.readTemperature();
  humidity = sht40.readHumidity();
  
  String data = String(temperature) + "," + String(humidity);
  String command = encryptCommand(data);
  
  radio.transmit((byte*)command.c_str(), command.length());
  delay(100);  // Для корректной отправки данных ожидаем небольшую паузу
}

void receiveCommands() {
  while (radio.available()) {
    int dataSize = radio.getPacketLength();
    byte data[dataSize];
  
    radio.receive(data, dataSize);
  
    String command = decryptCommand(String((char*)data));
    // Обработка команды...
  }
}

void loop() {
  sendTemperatureAndHumidity();
  receiveCommands();
  
  // Дополнительная логика работы, если требуется
}

void resetDevice() {
  // Выполнение программного ресета
  // Ниже приведен пример кода для AVR-микроконтроллеров с использованием библиотеки avr/wdt.h
  // Для других платформ и микроконтроллеров может потребоваться другой подход

  // Отключение прерываний
  noInterrupts();

  // Сброс системного таймера
  wdt_reset();

  // Установка режима сброса по истечении времени (WDT Timeout) в 16 миллисекунд
  // Длительность времени зависит от настроек WDT и может быть изменена
  wdt_enable(WDTO_15MS);

  // Зацикливание с целью сброса устройства
  while (1) {
    // ожидание сброса устройства...
  }
}

библиотека avr/wdt.h не работает с ESP32, потому что она предназначена для микроконтроллеров AVR, используемых в Arduino UNO и других платформах на основе AVR.

Для реализации программного сброса (watchdog reset) на ESP32 можно использовать встроенный таймер просмотра (watchdog timer) и функции из библиотеки esp32-hal.h.

Вот пример кода, демонстрирующий, как использовать таймер просмотра для программного сброса ESP32:

```cpp
#include <esp32-hal.h>

void setup() {
  // Включение таймера просмотра
  esp_task_wdt_init(1, true); // Задаем время срабатывания таймера (в секундах)
  esp_task_wdt_add(NULL); // Добавляем текущую задачу в список контролируемых задач
}

void loop() {
  // Ваш код выполнения

  // Пинг таймера просмотра
  esp_task_wdt_reset();
  
  // Остальная часть вашего кода
}
