#include "heltec.h"

#include <my_light_control.h>

My_LightControl mylight;

void setup()
{
    Serial.begin(115200);

    delay(3000);
    mylight.init();
}

void loop()
{
    mylight.light_ctrl_active();

  delay(1000);
}
