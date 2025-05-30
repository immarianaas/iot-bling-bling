#include <my_battery.h>
#include "Adafruit_MAX1704X.h"
#include <Wire.h>

#define SDA_PIN 9
#define SCL_PIN 2

#define BUZZER 18

void My_Battery::init()
{
    pinMode(BUZZER, OUTPUT);
    Wire.begin(SDA_PIN, SCL_PIN);

    while (!maxlipo.begin())
    {
        Serial.println(F("MAX17048 not found!"));
        delay(500);
    }

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
}

bool My_Battery::update_battery_x_sec()
{
    unsigned long now = millis();
    // Check battery every 60 seconds
    if (now - lastBatteryCheck >= BATTERY_UPDATE_INTERVAL)
    {
        lastBatteryCheck = now;
        updateBatteryStatus();
        return true;
    }
    return false;
}

void My_Battery::updateBatteryStatus()
{
    float obtainedPercent = maxlipo.cellPercent();

    if (isnan(obtainedPercent))
        batteryPercent = -1;
    else
        batteryPercent = (int)obtainedPercent;

    Serial.print("batteryPercent=");
    Serial.println(batteryPercent);
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

    if (now - lastBatteryCheck >= 10000)
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
    {
    }
}

void My_Battery::handle_battery()
{
    // first update the battery every 60 seconds
    bool updated = update_battery_x_sec();
    if (!updated)
        return;

    if (batteryPercent == -1) return;

    if (batteryPercent <= SECOND_ALARM_TRIGGER  && !secondAlarm)
    {
        secondAlarm = true;
        triggerAlarm();
    }
    else if (batteryPercent <= FIRST_ALARM_TRIGGER  && !firstAlarm)
    {
        firstAlarm = true;
        triggerAlarm();
    }

    if (secondAlarm && batteryPercent > SECOND_ALARM_TRIGGER)
    {
        secondAlarm = false;
    }
    else if (firstAlarm && batteryPercent > FIRST_ALARM_TRIGGER)
    {
        firstAlarm = false;
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


void My_Battery::request_buzzer()
{
    if (isBuzzing)
        return; // ignore request if it is buzzing already

    isBuzzing = true;
    buzzerStart = millis();
    tone(BUZZER, 1000);
}

void My_Battery::handle_buzzer()
{
    if (!isBuzzing) return; // nothing to do
    if (millis() - buzzerStart > BUZZER_REQ_TIME)
    {
        noTone(BUZZER);
        isBuzzing = false;
    }
}