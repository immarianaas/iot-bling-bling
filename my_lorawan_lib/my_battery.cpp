#include <my_battery.h>
#include "Adafruit_MAX1704X.h"
#include <Wire.h>

#define SDA_PIN 6
#define SCL_PIN 7

#define BUZZER 18

// My_Battery::My_Battery() : maxlipo()
// {
// }

void My_Battery::init()
{
    pinMode(BUZZER, OUTPUT);

    // Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setPins(SDA_PIN, SCL_PIN);
    //Wire.begin();
    delay(300);
    // maxlipo.begin(&Wire);
    // while (!maxlipo.begin())
    // {
    //     Serial.println("MAX17048 not found!");
    //     // triggerAlarm();
    // }

    Serial.println("Smart Bike Light Initialized");
    lastBatteryCheck = millis();
}

void My_Battery::prepare_battery_msg(uint8_t *msg_buffer, uint8_t &msg_size)
{
    msg_buffer[0] = 0x30;
    updateBatteryStatus();
    if (batteryPercent == -1)
    {
        msg_size = 1;
        return;
    }

    msg_size = 2;
    msg_buffer[1] = batteryPercent;
    return;
}

void My_Battery::updateBatteryStatus()
{
    float obtainedPercent = maxlipo.cellPercent();
    if (isnan(obtainedPercent))
        batteryPercent = -1;
    else
        batteryPercent = (int)obtainedPercent;
}

bool My_Battery::shouldUpdate()
{
    return millis() - last_send > BATTERY_SEND_INTERVAL;
}
void My_Battery::updateLastSendTime()
{
    last_send = millis();
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

void My_Battery::handleBatteryAlarm()
{
    if (batteryPercent != -1 && batteryPercent < lowBatteryThreshold)
    {}
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

