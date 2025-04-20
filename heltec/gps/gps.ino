
#include <HardwareSerial.h>
#include <TinyGPS++.h>

#define GPS_RX 6   // UART RX
#define GPS_TX 7   // UART TX

TinyGPSPlus gps;
HardwareSerial GPSerial(1);

void setup(){
  Serial.begin(115200);
  delay(3000);
  Serial.println("starting now");
  GPSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  GPSerial.setTimeout(3000);

}

bool get_coords(float &lat, float &lng) {

  int now = millis();
  int timeout = 1000 * 5; // 5 seconds
  while (GPSerial.available() > 0 and timeout - millis() > 0) {
    int c = GPSerial.read();

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

byte* encode_coord_val_hex(float val) {
  int normalized = (uint32_t)(((val + 90.0) / 180.0) * 16777215.0); // 16777216 = 2^(8*3) -1 (highest value written with 4 bytes
  
  static byte byteArr[3];
  byteArr[0] = (normalized >> 16) & 0xFF;
  byteArr[1] = (normalized >> 8) & 0xFF;
  byteArr[2] = normalized & 0xFF;
  return byteArr;
}

void prepare_message(bool valid, float lat, float lng, uint8_t* msg_buffer, uint8_t* msg_size) {
  if (!valid)
  {
    msg_size = 1;
    msg_buffer[0] = 0x10; // code for coordinates
    return;
  }

  byte[] latArr = encode_coord_val_hex(lat);
  byte[] lngArr = encode_coord_val_hex(lng);
  msg_size = 7;
  msg_buffer[0] = 0x10; // code for coordinates
  for (int i = 1; i<4; ++i)
  {
    msg_buffer[i] = latArr[i-1];
  }
  for (int i = 4; i<7; ++i)
  {
    msg_buffer[i] = lngArr[i-4];
  }
}

void loop() {

  float lat, lng;
  bool ok = get_coords(lat, lng);

  Serial.print("ok? ");
  Serial.println(ok);

  Serial.print("latitude: ");
  Serial.println(lat, 6);
  Serial.print("longitude: ");
  Serial.println(lng, 6);


  byte* latBytes = encode_coord_val_hex(lat);
  byte* lngBytes = encode_coord_val_hex(lng);

  // Print as hex
  Serial.print("Latitude (hex): ");
  for (int i = 0; i < 3; i++) {
    if (latBytes[i] < 0x10) Serial.print("0");  // leading zero
    Serial.print(latBytes[i], HEX);
  }
  Serial.println();

  delay(5000);
}



void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

