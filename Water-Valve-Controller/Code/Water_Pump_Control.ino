#include <WiFi.h>
#include <esp_now.h>

#define PUMP_RELAY_PIN 3
#define STATUS_LED_PIN 2

bool espNowInitialized = false;

// Replace with the MAC address of your transmitter ESP32
uint8_t transmitterAddress[] = {0xE4, 0xB0, 0x63, 0x41, 0x14, 0xC4};

void sendResponse(const char *response) {
  esp_err_t result = esp_now_send(transmitterAddress, (uint8_t *)response, strlen(response));
  if (result == ESP_OK) {
    Serial.println("Response sent successfully");
  } else {
    Serial.print("Error sending response: ");
    Serial.println(result);
  }
}

void onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  char msg[len + 1];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';
  Serial.print("Received message: ");
  Serial.println(msg);

  if (strcmp(msg, "PUMP_ON") == 0) {
    digitalWrite(PUMP_RELAY_PIN, LOW); // Relay ON
    Serial.println("Pump Relay ON");

    // Send acknowledgment back
    sendResponse("Pump switched on");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, HIGH); // Relay OFF
  
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  WiFi.mode(WIFI_STA);

  while (!espNowInitialized) {
    if (esp_now_init() == ESP_OK) {
      espNowInitialized = true;
      Serial.println("ESP-NOW Initialized Successfully");
      digitalWrite(STATUS_LED_PIN, HIGH);

      // Register receive callback
      esp_now_register_recv_cb(esp_now_recv_cb_t(onDataReceive));

      // Register peer (transmitter)
      esp_now_peer_info_t peerInfo;
      memcpy(peerInfo.peer_addr, transmitterAddress, 6);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
      
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add transmitter as peer");
      }
    } else {
      Serial.println("ESP-NOW Initialization Failed. Retrying in 2 seconds...");
      delay(2000);
    }
  }

  // System ready
  digitalWrite(STATUS_LED_PIN, HIGH);
  Serial.println("System Ready, Status LED ON");
}

void loop() {
  // Nothing in loop, waiting for ESP-NOW events
}
