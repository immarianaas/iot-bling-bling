#include <my_battery.h>
#include "Adafruit_MAX1704X.h"
#include <Wire.h>

#define SDA_PIN 6
#define SCL_PIN 7

#define BUZZER 18

void My_Battery::init()
{
    pinMode(BUZZER, OUTPUT);

    Wire.begin(SDA_PIN, SCL_PIN);
    delay(10); 

    if (!maxlipo.begin())
    {
        Serial.println(("MAX17048 not found!"));
        triggerAlarm();
    }

    Serial.println("Smart Bike Light Initialized");
    lastBatteryCheck = millis();
}

void My_Battery::checkBattery10Sec()
{
    unsigned long now = millis();
    // Check battery every 10 seconds
    
    if (now - lastBatteryCheck >= batteryCheckInterval)
    {
        lastBatteryCheck = now;
        checkBattery();
    }
}

void My_Battery::checkBattery()
{
    float voltage = maxlipo.cellVoltage();
    float percent = maxlipo.cellPercent();

    if (isnan(voltage))
    {
        Serial.println("Battery read error.");
        return;
    }

    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println(" V");
    Serial.print("Battery: ");
    Serial.print(percent);
    Serial.println(" %");

    if (percent < lowBatteryThreshold)
    {   
        triggerAlarm();
        Serial.println("LOW BATTERY! Triggering alarm and shutdown...");
    }
}

void My_Battery::triggerAlarm()
{
    for (int i = 0; i < 3; i++)
    {
        tone(BUZZER, 1000);
        delay(200);
        noTone(BUZZER);
        delay(200);
    }
}

