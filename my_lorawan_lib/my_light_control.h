#ifndef My_LIGHT_CONTROL_H
#define My_LIGHT_CONTROL_H
#include "DFRobot_ADXL345.h" //library for accelorometer
class My_LightControl
{
public:
    My_LightControl();
    void init();
    void light_ctrl_active();
    bool is_active();

private:
    void static IRAM_ATTR accelISR();
    void static IRAM_ATTR buttonISR();
    void button();
    bool is_dark();
    bool is_accel_activity();
    DFRobot_ADXL345_I2C ADXL345;
    bool isActive = false;               // is light active / mode active
    unsigned long lastMoveTime = 0;      // last time movement was detected
    unsigned long lastManualToggle = 0;  // last time button was pressed
    bool manual = 0;                     // indicates if light manually turned on
};
#endif // My_LIGHT_CONTROL_H
