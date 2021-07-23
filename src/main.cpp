// Bucket bridge via ESP-NOW
// Copyright (c) 2021 Inaba

#include <M5Atom.h>
#include <esp_now.h>
#include <WiFi.h>

namespace {

const int ID = 0;

const uint8_t target[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t CHANNEL = 0;

// Refer to Adafruit_NeoPixel/examples/strandtest_wheel
CRGB rgb(int i) {
  i += ID * 20;
  i %= 255;
  i = 255 - i;
  if (i < 85) {
    return CRGB(255 - i * 3, 0, i * 3);
  }
  if (i < 170) {
    i -= 85;
    return CRGB(0, i * 3, 255 - i * 3);
  }
  i -= 170;
  return CRGB(i * 3, 255 - i * 3, 0);
}

void OnDataRecv(const uint8_t * mac, const uint8_t *recvData, int len) {
  M5.dis.fillpix(rgb(*recvData));
  uint8_t i = *recvData + 1;
  ::delay(20);
  ::esp_now_send(target, &i, 1);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

}  // namespace

void setup() {
  M5.begin(true, false, true);
  M5.dis.fillpix(CRGB(0, 0, 0));
  WiFi.mode(WIFI_STA);
  Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

  if (::esp_now_init() != ESP_OK) {
    return;
  }
  ::esp_now_register_recv_cb(OnDataRecv);
  ::esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo;
  ::memcpy(peerInfo.peer_addr, target, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;

  if (::esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }
}

void loop() {
  M5.update();
  if (M5.Btn.wasPressed()) {
    // Start the relay.
    uint8_t i = 0;
    ::esp_now_send(target, &i, 1);
  }
}
