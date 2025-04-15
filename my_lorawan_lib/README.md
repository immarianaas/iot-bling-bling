This library is used for the LoraWAN. It was inspired by the 'LoRaWan_APP.h' and 'LoRaWan_APP.cpp' that comes with the Heltec library. This folder needs to be placed in the libraries folder of Arduino (in Linux this would probably be in ~/Arduino/libraries). The 'LoRaWan_APP' files probably need to be deleted from this folder too (/home/mar/Arduino/libraries/Heltec_ESP32_Dev-Boards/src).

I recommend making a simlink to the Arduino libraries folder. In linux the command would be:
`ln -s <path to my_lorawan_lib in this repo> <path to Arduino libraries>/my_lorawan_lib`
For example:
`ln -s ~/DTU/IOT/iot-bling-bling/my_lorawan_lib /home/mar/Arduino/libraries/my_lorawan_lib`
