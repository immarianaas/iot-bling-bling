#include "Adafruit_MAX1704X.h"
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_MAX17048 maxlipo;

const int buzzerPin = 25;
const int powerControlPin = 26;  // Controls JC2N3906 to power a module
const float lowBatteryThreshold = 30.0;

unsigned long lastBatteryCheck = 0;
const unsigned long batteryCheckInterval = 10000;  // 10 seconds

unsigned long powerShutdownTime = 0;
const unsigned long shutdownDelay = 30000;         // 30 seconds

bool powerIsOn = true;

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);
  pinMode(powerControlPin, OUTPUT);
  digitalWrite(powerControlPin, LOW);  

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!maxlipo.begin()) {
    Serial.println(F("MAX17048 not found!"));
    triggerAlarm();
    shutDown();
  }

  Serial.println("Smart Bike Light Initialized");

  lastBatteryCheck = millis();
  powerShutdownTime = millis() + shutdownDelay;  // Set time to turn off power
}

void loop() {
  unsigned long now = millis();

  // Check battery every 10 seconds
  if (now - lastBatteryCheck >= batteryCheckInterval) {
    lastBatteryCheck = now;
    checkBattery();
  }

  // Shut down after 30 seconds (non-blocking)
  if (powerIsOn && now >= powerShutdownTime) {
    Serial.println("Auto-shutdown after 30 seconds.");
    shutDown();
  }
}

void checkBattery() {
  float voltage = maxlipo.cellVoltage();
  float percent = maxlipo.cellPercent();

  if (isnan(voltage)) {
    Serial.println("Battery read error.");
    return;
  }

  Serial.print("Voltage: "); Serial.print(voltage); Serial.println(" V");
  Serial.print("Battery: "); Serial.print(percent); Serial.println(" %");

  if (percent < lowBatteryThreshold) {
    Serial.println("LOW BATTERY! Triggering alarm and shutdown...");
    triggerAlarm();
    shutDown();
  }
}

void triggerAlarm() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1000);
    delay(200); 
    noTone(buzzerPin);
    delay(200);
  }
}

void shutDown() {
  digitalWrite(powerControlPin, HIGH); 
  powerIsOn = false;
}
