#include <WiFi.h>
#include <esp_now.h>

// GPIO Definitions
#define LED_PIN      2   // Status LED (system alive)
#define RED_LED_PIN  1   // Red LED for Low Water Level or Idle State
#define VALVE_PIN    3   // Solenoid Valve
#define SENSOR_PIN   4   // XKC-Y26-V Output
#define BUTTON_PIN   5   // Push button

// Master (receiver) MAC Address – update this to match your master device
uint8_t receiverAddress[] = {0xE4, 0xB0, 0x63, 0x41, 0x14, 0xC4};

// Internal state variables
bool valveOpen = false;
bool lastButtonState = HIGH;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Active LOW

  digitalWrite(LED_PIN, HIGH);       // System alive indicator
  digitalWrite(RED_LED_PIN, LOW);    // Red LED off initially
  digitalWrite(VALVE_PIN, LOW);      // Valve initially closed

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_send_cb(esp_now_send_cb_t(onDataSent));

  // Register the peer (master)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_add_peer(&peerInfo)) {
    Serial.println("Peer Added");
  } else {
    Serial.println("Failed to add peer");
  }
}

void loop() {
  bool levelLow = digitalRead(SENSOR_PIN) == LOW;       // LOW when water level is low
  bool buttonState = digitalRead(BUTTON_PIN);           // HIGH = not pressed, LOW = pressed

  if (levelLow) {
    if (valveOpen) {
      Serial.println("Low Water Level Detected – Closing Valve!");
    }
    valveOpen = false;
    digitalWrite(VALVE_PIN, LOW);

    const char *msg = "Low Water Level";
    esp_now_send(receiverAddress, (uint8_t *)msg, strlen(msg));
  } else {
    if (lastButtonState == LOW && buttonState == HIGH) {
      Serial.println("Button Press Detected & Water Level OK -> Opening Valve");
      valveOpen = true;
    }

    digitalWrite(VALVE_PIN, valveOpen ? HIGH : LOW);
  }

  // 🔴 RED LED Control Logic
  if (levelLow || !valveOpen) {
    digitalWrite(RED_LED_PIN, HIGH);   // RED ON
  } else {
    digitalWrite(RED_LED_PIN, LOW);    // RED OFF
  }

  lastButtonState = buttonState;
  delay(50);  // small debounce delay
}
