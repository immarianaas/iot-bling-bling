#include <my_battery.h>
#include <my_light_control.h>
#include <my_gps.h>

My_Battery mybattery;
My_LightControl mylight;
My_GPS mygps;


void setup() {
    Serial.begin(115200);

    mybattery.init();
    mylight.init();
}
void loop() {
  mylight.light_ctrl_active();

  mybattery.checkBattery10Sec();
  // float lat, lng;
  // mygps.get_coords(lat, lng);

 //delay(500);
}
