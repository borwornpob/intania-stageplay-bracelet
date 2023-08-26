#include <esp_now.h>
#include <WiFi.h>

struct LEDData {
  int effects;
};

#define NUM_RECV 9

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

uint8_t broadcastAddress[NUM_RECV][6] = {
  {0xC0, 0x49, 0xEF, 0xF9, 0x05, 0x48},
  {0xB8, 0xD6, 0x1A, 0x68, 0x25, 0xC8},
  {0x40, 0x22, 0xD8, 0xFF, 0x7C, 0x04},
  {0x40, 0x22, 0xD8, 0xFF, 0xA0, 0xF0},
  {0xE8, 0x68, 0xE7, 0x2C, 0x0C, 0x24},
  {0xE0, 0x5A, 0x1B, 0x6B, 0x80, 0x4C},
  {0xE0, 0x5A, 0x1B, 0x6B, 0x77, 0x88},
  {0xB8, 0xD6, 0x1A, 0x68, 0x32, 0x64},
  {0xE0, 0x5A, 0x1B, 0x6B, 0x80, 0x74},
};

esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow Init Failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Add peers for broadcasting
  for (int i = 0; i < NUM_RECV; i++) {
    memcpy(peerInfo.peer_addr, broadcastAddress[i], 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }
  }
}

void loop() {
  if (Serial.available()) {
    LEDData data;
    String input = Serial.readString();
    int effect;
    char addresses[50];

    sscanf(input.c_str(), "%d:%s", &effect, addresses);
    data.effects = effect;

    Serial.println(input);

    char* token = strtok(addresses, ",");
    while(token != NULL) {
      int index = atoi(token);
      if(index >= 0) {
        esp_now_send(broadcastAddress[index-1], (uint8_t*)&data, sizeof(data));
      }
      token = strtok(NULL, ",");
    }
  }
}
