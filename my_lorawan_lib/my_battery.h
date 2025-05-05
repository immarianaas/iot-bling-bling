#ifndef My_BATTERY_H
#define My_BATTERY_H

#include "Adafruit_MAX1704X.h"

class My_Battery
{
public:
    My_Battery();
    void init();
    void checkBattery();
    void checkBattery10Sec();

private:
    void triggerAlarm();
    void shutDown();
    Adafruit_MAX17048 maxlipo;
    unsigned long lastBatteryCheck = 0;
    const unsigned long batteryCheckInterval = 10000;  // 10 seconds
};

#endif // My_BATTERY_H
