#include "WiFi.h"

void init() {
  // Set Wi-Fi mode to station to prevent ESP32 from acting as AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Disconnect from any network just in case
  delay(100); 
}

void find_networks() {
  int networksFound = WiFi.scanNetworks();

  if (networksFound == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("Found %d networks:\n", networksFound);
    for (int i = 0; i < networksFound; ++i) {
      Serial.printf(
        "SSID: %s | BSSID: %s | RSSI: %d | Channel: %d\n",
        WiFi.SSID(i).c_str(),
        WiFi.BSSIDstr(i).c_str(),
        WiFi.RSSI(i),
        WiFi.channel(i)
      );

      uint8_t bssid[6];
      WiFi.BSSID(i, bssid);
      uint8_t rssi_inv = - WiFi.RSSI(i);
      uint8_t channel = WiFi.channel(i);



    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Starting Wi-Fi scan...");

  // Set Wi-Fi mode to station to prevent ESP32 from acting as AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Disconnect from any network just in case
  delay(100);

  int networksFound = WiFi.scanNetworks();

  if (networksFound == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("Found %d networks:\n", networksFound);
    for (int i = 0; i < networksFound; ++i) {
      Serial.printf(
        "SSID: %s | BSSID: %s | RSSI: %d | Channel: %d\n",
        WiFi.SSID(i).c_str(),
        WiFi.BSSIDstr(i).c_str(),
        WiFi.RSSI(i),
        WiFi.channel(i)
      );
    }
  }

  // Optional: Clean up scan results
  WiFi.scanDelete();
}

void loop() {
  // Nothing to do here unless you want to scan periodically
}
