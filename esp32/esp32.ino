
#include <HardwareSerial.h>
#include <TinyGPS++.h>

#define GPS_RX 18   // UART RX
#define GPS_TX 19   // UART TX

TinyGPSPlus gps;
HardwareSerial GPSerial(2);

void setup(){
  Serial.begin(115200);
  delay(1000);
  GPSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  GPSerial.setTimeout(1000);
}

void loop() {

  while (GPSerial.available() > 0) {
    int c = GPSerial.read();
    Serial.write(c);

    if (gps.encode(c)) { Serial.println(); displayInfo();}
  }
  
  
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