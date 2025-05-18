#ifndef My_GPS_H
#define My_GPS_H

#include "Arduino.h"
#include <HardwareSerial.h>
#include <TinyGPS++.h>

class My_GPS
{
public:
    My_GPS();
    void init();
    void encode_coord_val_hex(float val, int *arr); /* debug */
    float decode_coord_val_hex(int *bytes);         /* debug */
    void print_hex_arr_values(int *arr);            /* debug */
    void prepare_coords_msg(uint8_t *msg_buffer, uint8_t &msg_size);
    void prepare_coords_msg_req(uint8_t *msg_buffer, uint8_t &msg_size);
    bool shouldUpdate(bool activeMode);
    void get_coords_init();
    bool got_valid_coords();

private:
    bool get_coords(float &lat, float &lng);
    void prepare_message(bool valid, float lat, float lng, uint8_t *msg_buffer, uint8_t &msg_size);
    bool get_coords_long(float &lat, float &lng, int minutes);
    TinyGPSPlus gps;
    HardwareSerial GPSerial;
    bool was_already_active = false;
    bool has_obtained_valid_coords = false;
};

#endif // My_GPS_H