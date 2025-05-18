#ifndef My_BATTERY_H
#define My_BATTERY_H

#include "Adafruit_MAX1704X.h"
// #define BATTERY_SEND_INTERVAL 2 * 60 * 1000 // 2 minutes
#define BATTERY_SEND_INTERVAL 5 * 1000 // 5 seconds

class My_Battery
{
public:
    // My_Battery();
    void init();
    // void init_old();
    void checkBattery();
    void checkBattery10Sec();
    bool shouldUpdate();
    void updateLastSendTime();
    void prepare_battery_msg(uint8_t *msg_buffer, uint8_t &msg_size);

private:
    void handleBatteryAlarm();
    void updateBatteryStatus();
    void triggerAlarm();
    // TwoWire i2c;
    Adafruit_MAX17048 maxlipo;
    const float lowBatteryThreshold = 30.0;
    unsigned long lastBatteryCheck = 0;
    const unsigned long batteryCheckInterval = 10000; // 10 seconds
    unsigned long last_send = 0;
    int batteryPercent = -1;
};

#endif // My_BATTERY_H
