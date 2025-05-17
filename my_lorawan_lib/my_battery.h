#ifndef My_BATTERY_H
#define My_BATTERY_H

#include "Adafruit_MAX1704X.h"
#define BATTERY_SEND_INTERVAL 2 * 60 * 1000 // 2 minutes

class My_Battery
{
public:
    My_Battery();
    void init();
    void checkBattery();
    void checkBattery10Sec();
    bool shouldUpdate();
    void updateLastSendTime();
    void prepare_battery_msg(uint8_t *msg_buffer, uint8_t &msg_size);

private:
    void handleBatteryAlarm();
    void updateBatteryStatus();
    void triggerAlarm();
    void shutDown();
    void turnOn();
    Adafruit_MAX17048 maxlipo;
    unsigned long lastBatteryCheck = 0;
    const unsigned long batteryCheckInterval = 10000; // 10 seconds
    unsigned long last_send = 0;
    int batteryPercent = -1;
};

#endif // My_BATTERY_H
