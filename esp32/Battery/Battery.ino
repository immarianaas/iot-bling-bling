#include "Adafruit_MAX1704X.h"
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_MAX17048 maxlipo;

const int buzzerPin = 25;
const float lowBatteryThreshold = 30.0;
const unsigned long batteryCheckInterval = 5000; // 5 seconds

unsigned long lastBatteryCheck = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);

  // Power to MAX17048 should be connected directly to 3.3V for this test
  Serial.println("Initializing I2C and MAX17048...");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  if (!maxlipo.begin()) {
    Serial.println("ERROR: MAX17048 not detected. Check wiring and power.");
    triggerAlarm();
    while (1); // Stop here if initialization fails
  }

  Serial.println("MAX17048 detected. Beginning battery read loop.");
  lastBatteryCheck = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastBatteryCheck >= batteryCheckInterval) {
    lastBatteryCheck = now;
    checkBattery();
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
    Serial.println("LOW BATTERY! Triggering alarm.");
    triggerAlarm();
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
