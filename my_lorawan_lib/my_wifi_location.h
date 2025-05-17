#ifndef MY_WIFI_LOCATION_H

#define MY_WIFI_LOCATION_H

#include "Arduino.h"
#define WIFI_SEND_INTERVAL 5 * 60 * 1000 // 5 minutes

class My_Wifi_Location
{
public:
    My_Wifi_Location();
    void init();
    void get_network_info(int index, uint8_t *arr);
    void print_network_info_hex(uint8_t *arr);
    void prepare_wifi_msg(uint8_t *msg_buffer, uint8_t &msg_size);
    int num_networks;
    bool shouldUpdate(bool activeMode);
    void updateLastSendTime();

private:
    unsigned long last_send = 0;
};

#endif // MY_WIFI_LOCATION_H