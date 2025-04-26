#include "my_wifi_location.h"

#include "WiFi.h"

My_Wifi_Location::My_Wifi_Location()
{

}

void My_Wifi_Location::init()
{
    // Set Wi-Fi mode to station to prevent ESP32 from acting as AP
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Disconnect from any network just in case
    delay(100);
    num_networks = WiFi.scanNetworks();
}

void My_Wifi_Location::get_network_info(int index, uint8_t *arr)
{
    uint8_t bssid[6];
    WiFi.BSSID(index, bssid);
    uint8_t rssi_inv = -WiFi.RSSI(index);
    uint8_t channel = WiFi.channel(index);

    for (int i = 0; i < 6; ++i)
        arr[i] = bssid[i];
    arr[6] = rssi_inv; // fits into a byte
    arr[7] = channel;  // fits into a byte too
}

void My_Wifi_Location::print_network_info_hex(uint8_t *arr)
{
    Serial.print("BSSID (0..5): ");
    for (int i = 0; i < 6; i++)
    {
        if (arr[i] < 0x10)
            Serial.print("0"); // leading zero
        Serial.print(arr[i], HEX);
    }
    Serial.println();

    Serial.print("RSSI inverted (6): ");
    if (arr[6] < 0x10)
        Serial.print("0");
    Serial.println(arr[6], HEX);

    Serial.print("Channel (7): ");
    if (arr[7] < 0x10)
        Serial.print("0");
    Serial.println(arr[7], HEX);
}

void My_Wifi_Location::prepare_wifi_msg(uint8_t *msg_buffer, uint8_t &msg_size)
{
    num_networks = WiFi.scanNetworks();

    msg_buffer[0] = 0x20; // code for wifi scanning
    msg_size = 0;
    // max number of networks that fit a msg is 31, so we cannot send more than that
    for (int i = 0; i < num_networks && i < 31; ++i)
    {
        uint8_t array[8];
        get_network_info(i, array);
        for (int j = 0; j < 8; ++j)
            msg_buffer[++msg_size] = array[j];
    }
    ++msg_size;
}
