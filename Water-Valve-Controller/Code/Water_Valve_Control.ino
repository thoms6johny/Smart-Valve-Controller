#include <WiFi.h>
#include <esp_now.h>

// GPIO Assignments
#define VALVE_1_PIN      2     // Valve 1 control
#define VALVE_2_PIN      3     // Valve 2 control
#define STAT_PIN         0     // LTC4412 STAT pin
#define SYSTEM_LED       1     // Always ON when system is ready
#define AC_STATUS_LED    4     // ON when AC power is present

// Receiver ESP32 MAC Address – replace with actual
uint8_t receiverAddress[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};  

bool lastPowerStatus = false;
bool awaitingAck = false;
unsigned long lastSendTime = 0;

// Function to send "PUMP_ON"
void sendPumpOnCommand() {
  const char *msg = "PUMP_ON";
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg, strlen(msg));
  if (result == ESP_OK) {
    Serial.println("Sent: PUMP_ON");
  } else {
    Serial.print("Send error: ");
    Serial.println(result);
  }
}

// Callback when message is received from receiver
void onDataReceive(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char msg[len + 1];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';

  Serial.print("Received from receiver: ");
  Serial.println(msg);

  if (strcmp(msg, "Pump switched on") == 0) {
    Serial.println("Acknowledgment received — stopping repeated sends.");
    awaitingAck = false; // Stop sending
  }
}

void setup() {
  Serial.begin(115200);

  // Pin modes
  pinMode(VALVE_1_PIN, OUTPUT);
  pinMode(VALVE_2_PIN, OUTPUT);
  pinMode(STAT_PIN, INPUT);
  pinMode(SYSTEM_LED, OUTPUT);
  pinMode(AC_STATUS_LED, OUTPUT);

  // Initial state
  digitalWrite(VALVE_1_PIN, LOW);
  digitalWrite(VALVE_2_PIN, LOW);
  digitalWrite(SYSTEM_LED, HIGH);    // System is running
  digitalWrite(AC_STATUS_LED, LOW);  // Default OFF until AC is detected

  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  // Register receive callback
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataReceive));

  // Add receiver peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Detect initial power source
  lastPowerStatus = (digitalRead(STAT_PIN) == LOW); // LOW = AC present
  if (lastPowerStatus) {
    Serial.println("AC Power Present");
    digitalWrite(AC_STATUS_LED, HIGH);
    digitalWrite(VALVE_1_PIN, HIGH);
    digitalWrite(VALVE_2_PIN, LOW);
    awaitingAck = true; // Start sending messages
  } else {
    Serial.println("Battery Power Mode");
    digitalWrite(AC_STATUS_LED, LOW);
    digitalWrite(VALVE_1_PIN, LOW);
    digitalWrite(VALVE_2_PIN, HIGH);
  }
}

void loop() {
  bool currentPowerStatus = (digitalRead(STAT_PIN) == LOW);  // LOW means AC power available

  if (currentPowerStatus != lastPowerStatus) {
    // Power source changed
    Serial.print("Power Source Switched: ");
    Serial.println(currentPowerStatus ? "AC Power" : "Battery");

    if (currentPowerStatus) {
      // AC Restored
      digitalWrite(VALVE_1_PIN, HIGH);
      digitalWrite(VALVE_2_PIN, LOW);
      digitalWrite(AC_STATUS_LED, HIGH);
      awaitingAck = true; // Start sending messages
      lastSendTime = 0;    // Reset timer so it sends immediately
    } else {
      // Battery mode
      digitalWrite(VALVE_1_PIN, LOW);
      digitalWrite(VALVE_2_PIN, HIGH);
      digitalWrite(AC_STATUS_LED, LOW);
      awaitingAck = false; // Stop sending
    }

    lastPowerStatus = currentPowerStatus;
  }

  // If AC power is on and acknowledgment not yet received, send every 2 seconds
  if (awaitingAck && millis() - lastSendTime >= 2000) {
    sendPumpOnCommand();
    lastSendTime = millis();
  }

  delay(50);
}
