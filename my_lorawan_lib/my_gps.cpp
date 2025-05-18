#include <my_gps.h>
// #include <HardwareSerial.h>
// #include <TinyGPS++.h>

// #define GPS_RX 6 // UART RX
// #define GPS_TX 7 // UART TX

// tx was (wrongly) connected to 3
// rx was/is connected to 15 (which is 0)

#define GPS_RX 0 // 2
#define GPS_TX 3

#define SEARCH_DATA_MINUTES 1
// 3 is connected to RX on the GPS sensor!!!
// 0 is connected to TX on the GPS sensor!!
// #define GPS_RX 2
// #define GPS_TX 0

My_GPS::My_GPS() : gps(), GPSerial(1)
{
    GPSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    GPSerial.setTimeout(3000);
}

void My_GPS::init()
{
    float lat, lng;
    get_coords_long(lat, lng, SEARCH_DATA_MINUTES);
}

void My_GPS::get_coords_init()
{
    float lat, lng;
    has_obtained_valid_coords = get_coords(lat, lng);
}

bool My_GPS::got_valid_coords() {
    return has_obtained_valid_coords;
}

bool My_GPS::get_coords_long(float &lat, float &lng, int minutes)
{
    unsigned long now = millis();
    Serial.println("looking for coordinates...");
    bool ok = false;
    while (millis() - now < minutes * 60 * 1000 && !ok) // maximum x minutes
        ok = get_coords(lat, lng);
    
    if (ok) Serial.println("got coordinates!");
    return ok;
}

bool My_GPS::get_coords(float &lat, float &lng)
{
    int now = millis();
    int timeout = 1000 * 5; // 5 seconds
    while (GPSerial.available() > 0 and timeout - millis() > 0)
    {
        int c = GPSerial.read();
        Serial.print(c);
        Serial.println("here");
        gps.encode(c);
        if (gps.location.isValid())
        {
            lat = gps.location.lat();
            lng = gps.location.lng();
            return true;
        }
    }
    return false;
}

void My_GPS::encode_coord_val_hex(float val, int *arr)
{
    int normalized = (uint32_t)(((val + 90.0) / 180.0) * 16777215.0); // 16777216 = 2^(8*3) -1 (highest value written with 4 bytes

    arr[0] = (normalized >> 16) & 0xFF;
    arr[1] = (normalized >> 8) & 0xFF;
    arr[2] = normalized & 0xFF;
}

float My_GPS::decode_coord_val_hex(int *bytes)
{
    // Combine the 3 bytes into a single 24-bit integer
    int normalized = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];

    // Reverse the normalization
    float val = ((float)normalized / 16777215.0f) * 180.0f - 90.0f;

    return val;
}

void My_GPS::prepare_message(bool valid, float lat, float lng, uint8_t *msg_buffer, uint8_t &msg_size)
{
    if (!valid)
    {
        msg_size = 1;
        msg_buffer[0] = 0x10; // code for coordinates
        return;
    }

    int latArr[3];
    int lngArr[3];
    encode_coord_val_hex(lat, latArr);
    encode_coord_val_hex(lng, lngArr);

    msg_size = 7;
    msg_buffer[0] = 0x10; // code for coordinates
    for (int i = 1; i < 4; ++i)
    {
        msg_buffer[i] = latArr[i - 1];
    }
    for (int i = 4; i < 7; ++i)
    {
        msg_buffer[i] = lngArr[i - 4];
    }
}

void My_GPS::print_hex_arr_values(int *arr)
{
    for (int i = 0; i < 3; i++)
    {
        if (arr[i] < 0x10)
            Serial.print("0"); // leading zero
        Serial.print(arr[i], HEX);
    }
    Serial.println();
}

void My_GPS::prepare_coords_msg(uint8_t *msg_buffer, uint8_t &msg_size)
{
    float lat, lng;
    bool ok = get_coords(lat, lng);
    prepare_message(ok, lat, lng, msg_buffer, msg_size);
}

void My_GPS::prepare_coords_msg_req(uint8_t *msg_buffer, uint8_t &msg_size)
{
    float lat, lng;
    bool ok = get_coords_long(lat, lng, SEARCH_DATA_MINUTES);
    prepare_message(ok, lat, lng, msg_buffer, msg_size);
}

bool My_GPS::shouldUpdate(bool activeMode)
{
    if (!activeMode)
    {
        was_already_active = false;
        return false;
    }
    // if activeMode is true:
    // true:  if it wasnt active before
    // false: if it was active before
    bool ret = !was_already_active;
    was_already_active = true;
    return ret;
}
