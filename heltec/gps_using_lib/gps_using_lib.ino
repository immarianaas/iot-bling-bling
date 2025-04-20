#include <my_gps.h>

My_GPS mygps;

void setup() {
  Serial.begin(115200);
  delay(3000);

  mygps.init();
}

void loop() {
  float lat, lng;
  bool ok = mygps.get_coords(lat, lng);
  
  Serial.print("ok? ");
  Serial.println(ok);

  Serial.print("latitude: ");
  Serial.println(lat, 6);
  Serial.print("longitude: ");
  Serial.println(lng, 6);


  int latBytes[3];
  int lngBytes[3];  
  mygps.encode_coord_val_hex(lat, latBytes);
  mygps.encode_coord_val_hex(lng, lngBytes);

  mygps.print_hex_arr_values(latBytes);
  mygps.print_hex_arr_values(lngBytes);

  float conv_lat = mygps.decode_coord_val_hex(latBytes);
  Serial.print("converted latitude: ");
  Serial.println(conv_lat, 6);

  float conv_lng = mygps.decode_coord_val_hex(lngBytes);
  Serial.print("converted longitude: ");
  Serial.println(conv_lng, 6);

  delay(10000);

}
