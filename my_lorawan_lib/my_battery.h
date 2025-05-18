#ifndef My_BATTERY_H
#define My_BATTERY_H

#include "Adafruit_MAX1704X.h"
// #define BATTERY_SEND_INTERVAL 2 * 60 * 1000 // 2 minutes
#define BATTERY_SEND_INTERVAL 120000 // 120 seconds
#define BATTERY_UPDATE_INTERVAL 60000 // 60 seconds

#define FIRST_ALARM_TRIGGER 20
#define SECOND_ALARM_TRIGGER 10

#define BUZZER_REQ_TIME 5000 // 5 seconds

class My_Battery
{
public:
    // My_Battery();
    void init();
    // void init_old();
    void checkBattery();
    void checkBattery10Sec();
    void handle_battery();
    
    bool shouldUpdate();
    void updateLastSendTime();
    void prepare_battery_msg(uint8_t *msg_buffer, uint8_t &msg_size);

    void request_buzzer();
    void handle_buzzer();

private:
    bool update_battery_x_sec();
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
    bool firstAlarm = false; // has the alarm played?
    bool secondAlarm = false;

    bool isBuzzing = false;
    unsigned long buzzerStart = 0;
    bool buzzer_request = false;
};

#endif // My_BATTERY_H
