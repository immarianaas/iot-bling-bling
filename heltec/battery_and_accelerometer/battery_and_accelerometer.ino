#include <my_battery.h>
#include <my_light_control.h>

My_Battery mybattery;
My_LightControl mylight;


void setup() {
    Serial.begin(115200);

    mybattery.init();
    mylight.init();
}
void loop() {
  mylight.light_ctrl_active();

  mybattery.checkBattery10Sec();

 //delay(500);
}
