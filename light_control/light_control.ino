#include <Wire.h>             //I2C
#include "DFRobot_ADXL345.h" //library for accelorometer

int ledPin = LED_BUILTIN;  //led output
     
const int photoPin = 36;  // GPIO36 (VP)
bool tap = 0;             //indicates if TAP detected
bool manual = 0;          //indicates if light manually turned on

unsigned long lastMoveTime = 0;               //timer for light turn off after x time
const int stillDelay = 3000;                  // delay before light goes off in milliseconds

DFRobot_ADXL345_I2C ADXL345(&Wire,0x53);

void setup() {
    Serial.begin(57600);
    pinMode(ledPin,  OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    ADXL345.begin();
    ADXL345.powerOn();
    
    //turn on activity sensing for accelorometer
    ADXL345.setActivityX(true);
    ADXL345.setActivityY(true);
    ADXL345.setActivityZ(true);
  
    ADXL345.setActivityThreshold(15);  // Value from 0–255 (~62.5mg per LSB at ±2g)
    ADXL345.setFullResBit(true);
    ADXL345.setRangeSetting(2);          // ±2g range
    ADXL345.activityInt(true);  // Enable activity interrupt system

    // Detect taps in the directions turned ON(1 == ON, 0 == OFF)
    ADXL345.setTapDetectionOnX(0);
    ADXL345.setTapDetectionOnY(0);
    ADXL345.setTapDetectionOnZ(1);
    
    ADXL345.setTapThreshold(20); //tap threshhold, low = more sensitive 
    ADXL345.setTapDuration(15);  //tap duration

    ADXL345.singleTapInt(1);    //enable tap interrupt
}

void loop() {

    byte interrupts = ADXL345.getInterruptSource(); //reset interupts
    int value = analogRead(photoPin);               //read light sensor

    Serial.println("Analog  Value: ");
    Serial.println(value);
    
    if (tap == true) {              //if tap sensed, manage manual on/off
      if (manual == 0) {
        digitalWrite(ledPin, HIGH);
        manual = 1;
      } else {
      digitalWrite(ledPin, LOW);
        manual = 0;
      }
    tap = false;
    
    } else if (manual == true) {                                        //if manual on, keep on till manual off
      digitalWrite(ledPin, HIGH);
    
    } else if ((value < 2100) and (ADXL345.isActivitySourceOnX()or      //else if dark and moving, turn light on
                              ADXL345.isActivitySourceOnY()or 
                              ADXL345.isActivitySourceOnZ())) {         // threshold 2100 relatively good for 10k resistor 
        digitalWrite(ledPin, HIGH);
        Serial.println("Moving and Dark");
        lastMoveTime = millis(); // reset the timer
        
    } else if (millis() - lastMoveTime > stillDelay){                 //else if time elapsed since move, turn light off
        digitalWrite(ledPin,  LOW);
        Serial.println("Light off");
    }

    // Tap Detection
    if(ADXL345.triggered(interrupts, ADXL345_SINGLE_TAP)){          //tap detection interrupt
      Serial.println("*** TAP ***");
      tap = true;
    }
    Serial.println(manual);
    delay(500);
}
