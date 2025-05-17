#ifndef My_BATTERY_H
#define My_BATTERY_H

#include "Adafruit_MAX1704X.h"

class My_Battery
{
public:
    //My_Battery();
    void init();
    void checkBattery();
    void checkBattery10Sec();

private:
    void triggerAlarm();
    Adafruit_MAX17048 maxlipo;
    const float lowBatteryThreshold = 30.0;
    unsigned long lastBatteryCheck = 0;
    const unsigned long batteryCheckInterval = 100000;  // 10 seconds
};

#endif // My_BATTERY_H
