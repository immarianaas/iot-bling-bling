#ifndef My_LIGHT_CONTROL_H
#define My_LIGHT_CONTROL_H
#include "DFRobot_ADXL345.h" //library for accelorometer

class My_LightControl
{
public:
    My_LightControl();
    void init();
    void light_ctrl_active();

private:
    void static IRAM_ATTR accelISR();
    void static IRAM_ATTR buttonISR();
    DFRobot_ADXL345_I2C ADXL345;
};

#endif // My_LIGHT_CONTROL_H
