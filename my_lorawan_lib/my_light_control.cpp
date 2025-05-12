#include <my_light_control.h>

#define SDA_PIN 6
#define SCL_PIN 7

// const int ledPin = 5; // led output

#define LIGHT 8 // three kinda works
#define PHOTO 4
#define BUTTON 5
#define ACCEL_INTR 19 // dunno yet

volatile bool motionDetected = false;
volatile bool buttonPressed = false;

bool lastButtonState = HIGH; // Previous button state
bool manual = 0;             // indicates if light manually turned on
int lightValue = 0;

unsigned long lastMoveTime = 0; // timer for light turn off after x time
unsigned long lastManualToggle = 0;

const int stillDelay = 10000; // delay before light goes off in milliseconds
const int debounceTime = 200;

My_LightControl::My_LightControl() : ADXL345(&Wire, 0x53)
{
}

void My_LightControl::init()
{
    pinMode(LIGHT, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(ACCEL_INTR, INPUT_PULLUP);

    Wire.setPins(SDA_PIN, SCL_PIN);

    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(ACCEL_INTR), accelISR, RISING);

    ADXL345.begin();
    ADXL345.powerOn();

    ADXL345.setFullResBit(true);
    ADXL345.activityInt(true); // Enable activity interrupt system

    // turn on activity sensing for accelorometer
    ADXL345.setActivityX(true);
    ADXL345.setActivityY(true);
    ADXL345.setActivityZ(true);

    ADXL345.setActivityThreshold(15); // Value from 0–255 (~62.5mg per LSB at ±2g)
    ADXL345.setRangeSetting(2);       // ±2g range
    ADXL345.setRate(12.5);
    ADXL345.setLowPower(1);
}

void IRAM_ATTR My_LightControl::accelISR()
{
    motionDetected = true;
}

void IRAM_ATTR My_LightControl::buttonISR()
{
    buttonPressed = true;
}

void My_LightControl::light_ctrl_active()
{
    // Serial.print("ADXL345.isActivitySourceOnX()");
    // Serial.println(ADXL345.isActivitySourceOnX());

    // Serial.print("ADXL345.isActivitySourceOnY()");
    // Serial.println(ADXL345.isActivitySourceOnY());

    // Serial.print("ADXL345.isActivitySourceOnZ()");
    // Serial.println(ADXL345.isActivitySourceOnZ());


    unsigned long currentTime = millis();
    byte interrupts = ADXL345.getInterruptSource();

    lightValue = analogRead(PHOTO);
    // Serial.print("Light level: ");
    // Serial.println(lightValue);

    if (buttonPressed && currentTime - lastManualToggle > debounceTime)
    {
        manual = !manual;
        digitalWrite(LIGHT, manual ? HIGH : LOW);
        Serial.print("turning light to: ");
        Serial.println(manual ? HIGH : LOW);

        lastManualToggle = currentTime;
        buttonPressed = false;
        Serial.print("Manual mode: ");
        Serial.println(manual);
    }

    else if (!manual)
    {
        if (motionDetected)
        {
            motionDetected = false;
            if (ADXL345.triggered(interrupts, ADXL345_ACTIVITY))
            {
                Serial.println("Motion");
                if (lightValue < 2600)
                {
                    digitalWrite(LIGHT, HIGH);
                    lastMoveTime = currentTime;
                    Serial.println("Motion and dark: LED ON");
                }
            }
        }
        else if (currentTime - lastMoveTime > stillDelay)
        {
            digitalWrite(LIGHT, LOW);
            Serial.println("Still too long: LED OFF");
        }
    }
}
