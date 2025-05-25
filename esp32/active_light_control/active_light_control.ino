#include <Wire.h>             //I2C
#include "DFRobot_ADXL345.h" //library for accelorometer

const int ledPin = 5;  //led output     
const int photoPin = 36;  // GPIO36 (VP)
const int buttonPin = 18;
const int accel_int_pin = 19;

volatile bool motionDetected = false;
volatile bool buttonPressed = false;

bool lastButtonState = HIGH;  // Previous button state
bool manual = 0;              //indicates if light manually turned on
int lightValue = 0;

unsigned long lastMoveTime = 0;               //timer for light turn off after x time
unsigned long lastManualToggle = 0;

const int stillDelay = 3000;                  // delay before light goes off in milliseconds
const int debounceTime = 200;

DFRobot_ADXL345_I2C ADXL345(&Wire,0x53);


void IRAM_ATTR accelISR() {
  motionDetected = true;
}

void IRAM_ATTR buttonISR() {
  buttonPressed = true;
}

void light_ctrl_active() {
  unsigned long currentTime = millis();
  byte interrupts = ADXL345.getInterruptSource();

  lightValue = analogRead(photoPin);
  //Serial.print("Light level: ");
  //Serial.println(lightValue);

  if(buttonPressed && currentTime - lastManualToggle > debounceTime) {
    manual = !manual;
    digitalWrite(ledPin, manual ? HIGH : LOW);
    lastManualToggle = currentTime;
    buttonPressed = false;
    Serial.print("Manual mode: ");
    Serial.println(manual);
  }

  if (!manual) {
    if (motionDetected) {
      motionDetected = false;
      if (ADXL345.triggered(interrupts, ADXL345_ACTIVITY)) {
        Serial.println("Motion");
        if (lightValue < 2600) {
          digitalWrite(ledPin, HIGH);
          lastMoveTime = currentTime;
          Serial.println("Motion and dark: LED ON");
        }
      }
    }
    if (currentTime - lastMoveTime > stillDelay) {
      digitalWrite(ledPin, LOW);
      //Serial.println("Still too long: LED OFF");
    }
  }
}

void setup() {
    Serial.begin(57600);
    pinMode(ledPin,  OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(accel_int_pin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(accel_int_pin), accelISR, RISING);

    ADXL345.begin();
    ADXL345.powerOn();
    
    ADXL345.setFullResBit(true);
    ADXL345.activityInt(true);  // Enable activity interrupt system
    
    //turn on activity sensing for accelorometer
    ADXL345.setActivityX(true);
    ADXL345.setActivityY(true);
    ADXL345.setActivityZ(true);
  
    ADXL345.setActivityThreshold(15);  // Value from 0–255 (~62.5mg per LSB at ±2g)
    ADXL345.setRangeSetting(2);          // ±2g range
    ADXL345.setRate(12.5);
    ADXL345.setLowPower(1);
   

}
void loop() {
  light_ctrl_active();

 //delay(500);
}
