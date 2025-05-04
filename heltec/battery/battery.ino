#include "Adafruit_MAX1704X.h"

#define SCL_PIN 7    // 18 does something
#define SDA_PIN 6

Adafruit_MAX17048 maxlipo;

const int buzzerPin = 25;         // Buzzer pin
const int powerHoldPin = 33;      // GPIO to hold MOSFET ON (via diode to gate)
const float lowBatteryThreshold = 30.0; // 30%

void setup() {
  Wire.setPins(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);

  Serial.println(F("MAX17048 Battery Monitor with Shutdown Logic"));

  while (!maxlipo.begin()) {
    Serial.println(F("Couldn't find MAX17048! Is battery connected?"));
    delay(2000);
  }

  Serial.print(F("Found MAX17048, Chip ID: 0x"));
  Serial.println(maxlipo.getChipID(), HEX);
}

void loop() {
  float cellVoltage = maxlipo.cellVoltage();
  float batteryPercent = maxlipo.cellPercent();

  if (isnan(cellVoltage)) {
    Serial.println("Error reading battery!");
    delay(2000);
    return;
  }

  Serial.print(F("Voltage: ")); Serial.print(cellVoltage, 3); Serial.println(" V");
  Serial.print(F("Battery: ")); Serial.print(batteryPercent, 1); Serial.println(" %");

  if (batteryPercent < lowBatteryThreshold) {
    Serial.println("LOW BATTERY! Triggering alarm and shutdown...");
    triggerAlarm();
  } else {
    noTone(buzzerPin);
  }

  delay(2000);
}

void triggerAlarm() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1000);
    delay(200);
    noTone(buzzerPin);
    delay(200);
  }
}
