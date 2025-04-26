#ifndef My_GPS_H
#define My_GPS_H

#include "Arduino.h"
#include <HardwareSerial.h>
#include <TinyGPS++.h>

class My_GPS
{
public:

    My_GPS();
    bool get_coords(float &lat, float &lng);
    void encode_coord_val_hex(float val, int*arr);
    float decode_coord_val_hex(int *bytes);
    void prepare_message(bool valid, float lat, float lng, uint8_t *msg_buffer, uint8_t &msg_size);
    void print_hex_arr_values(int *arr);
    void prepare_coords_msg(uint8_t *msg_buffer, uint8_t &msg_size);

private:
    TinyGPSPlus gps;
    HardwareSerial GPSerial;
};

#endif // My_GPS_H