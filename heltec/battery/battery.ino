#include <my_battery.h>

My_Battery mybattery;

void setup()
{
  Serial.begin(115200);
  delay(500);
  mybattery.init();
}

void loop()
{
  mybattery.checkBattery10Sec();
}
