/*
  Project name: 5_Buzzer
  Function: buzzer plays Flat DO，Re，Mi，Fa，So，La，Si
  Author: keyestudio
  Hardware connection:
Connect to port 3 
Library:
none (no need to import additional library)
Cautions:
Ensure connect to port 3
Before uploading test code, please correctly connect to the development board and port
*/
#define beeppin 20  //Set pin to IO26

void setup() {
  pinMode(beeppin, OUTPUT);  //Set the pin to output
}

void loop() {
  tone(beeppin, 262);  //Flat DO plays 500ms
  delay(500);
  tone(beeppin, 294);  //Flat Re plays 500ms
  delay(500);
  tone(beeppin, 330);  //Flat Mi plays 500ms
  delay(500);
  tone(beeppin, 349);  //Flat Fa plays 500ms
  delay(500);
  tone(beeppin, 392);  //Flat So plays 500ms
  delay(500);
  tone(beeppin, 440);  //Flat La plays 500ms
  delay(500);
  tone(beeppin, 494);  //Flat Si plays 500ms
  delay(500);
  noTone(beeppin);  //stop playing
  delay(1000);
}
