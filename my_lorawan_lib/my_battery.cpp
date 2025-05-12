#include <my_battery.h>
#include "Adafruit_MAX1704X.h"
#include <Wire.h>

#define SDA_PIN 6
#define SCL_PIN 7

#define BUZZER 9 //18
#define POWERCONTROL 18 // 9

// to change later:
const float lowBatteryThreshold = 30.0;


unsigned long lastBatteryCheck = 0;
const unsigned long batteryCheckInterval = 10000; // 10 seconds

unsigned long powerShutdownTime = 0;
const unsigned long shutdownDelay = 30000; // 30 seconds

bool powerIsOn = true;

My_Battery::My_Battery() : maxlipo()
{
}

void My_Battery::init()
{
    pinMode(BUZZER, OUTPUT);
    pinMode(POWERCONTROL, OUTPUT);
    digitalWrite(POWERCONTROL, HIGH);

    Wire.begin(SDA_PIN, SCL_PIN);

    if (!maxlipo.begin())
    {
        Serial.println(F("MAX17048 not found!"));
        triggerAlarm();
        shutDown();
    }

    Serial.println("Smart Bike Light Initialized");
    lastBatteryCheck = millis();
    // powerShutdownTime = millis() + shutdownDelay;  // Set time to turn off power
}

void My_Battery::checkBattery10Sec()
{
    unsigned long now = millis();
    // Check battery every 10 seconds
    
    if (now - lastBatteryCheck >= batteryCheckInterval)
    {
        lastBatteryCheck = now;

        turnOn();
        checkBattery();
        shutDown();

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
        Serial.println("LOW BATTERY! Triggering alarm and shutdown...");
        triggerAlarm();
        shutDown();
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

void My_Battery::shutDown()
{
    digitalWrite(POWERCONTROL, HIGH);
    powerIsOn = false;
}

void My_Battery::turnOn() {
    digitalWrite(POWERCONTROL, LOW); 
    delay(100); 
    Wire.begin(SDA_PIN, SCL_PIN); 
    delay(10); 
    maxlipo.begin(); 
    powerIsOn = true;
}

