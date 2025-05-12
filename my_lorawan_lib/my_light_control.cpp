#include <my_light_control.h>

#define SDA_PIN 6
#define SCL_PIN 7

// const int ledPin = 5; // led output

#define PHOTO 4
#define BUTTON 5
#define ACCEL_INTR 19 // dunno yet

volatile bool accelDetected = false;
volatile bool buttonPressed = false;

bool lastButtonState = HIGH; // Previous button state
bool manual = 0;             // indicates if light manually turned on
int lightValue = 0;

unsigned long lastMoveTime = 0; // timer for light turn off after x time
unsigned long lastManualToggle = 0;

const int stillDelay = 3000; // delay before light goes off in milliseconds
const int debounceTime = 200;

My_LightControl::My_LightControl() : ADXL345(&Wire, 0x53)
{
}

void My_LightControl::init()
{
    // pinMode(ledPin, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(ACCEL_INTR, INPUT_PULLUP);

    Wire.setPins(SDA_PIN, SCL_PIN);

    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(ACCEL_INTR), accelISR, RISING);

    ADXL345.begin();
    ADXL345.powerOn();

    ADXL345.setFullResBit(true);
    ADXL345.activityInt(true); // Enable activity interrupt system
    ADXL345.inactivityInt(true);

    // turn on activity sensing for accelorometer
    ADXL345.setActivityX(true);
    ADXL345.setActivityY(true);
    ADXL345.setActivityZ(true);
    
    ADXL345.setInactivityX(true);
    ADXL345.setInactivityY(true);
    ADXL345.setInactivityZ(true);


    ADXL345.setActivityThreshold(15); // Value from 0–255 (~62.5mg per LSB at ±2g)
    ADXL345.setInactivityThreshold(10); 
    ADXL345.setTimeInactivity(5);       // 5 seconds
    ADXL345.setRangeSetting(2);       // ±2g range
    ADXL345.setRate(12.5);
    ADXL345.setLowPower(1);
}

void IRAM_ATTR My_LightControl::accelISR()
{
    accelDetected = true;
}


void IRAM_ATTR My_LightControl::buttonISR()
{
    buttonPressed = true;
}

void My_LightControl::button()
{   
    unsigned long currentTime = millis();

    if (buttonPressed && currentTime - lastManualToggle > debounceTime)
    {
        manual = !manual;
        digitalWrite(ledPin, manual ? HIGH : LOW);
        lastManualToggle = currentTime;
        buttonPressed = false;
        Serial.print("Manual mode: ");
        Serial.println(manual);
    }

}

bool My_LightControl::accel_activity()
{   
    byte interrupts = ADXL345.getInterruptSource();

    if (!manual)
    {
        if (accelDetected)
        {
            accelDetected = false;

            if (ADXL345.triggered(interrupts, ADXL345_ACTIVITY))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}


bool My_LightControl::accel_inactivity()
{   
    byte interrupts = ADXL345.getInterruptSource();

    if (!manual)
    {
        if (accelDetected)
        {
            accelDetected = false;

            if (ADXL345.triggered(interrupts, ADXL345_INACTIVITY))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

bool My_LightControl::dark()
{   
    if (!manual)
    {
        lightValue = analogRead(PHOTO);

        if (lightValue < 2600)
        {
        return true;
        }   
    }
    else
    {
    return false;
    }
}


void My_LightControl::light_ctrl_active()
{
    unsigned long currentTime = millis();

    button():

    if (accel_activity() and dark())
    {
        digitalWrite(ledPin, HIGH);
        lastMoveTime = currentTime;
        Serial.println("Motion and dark: LED ON");
    }

    if (!manual)
    {
        if (currentTime - lastMoveTime > stillDelay)
        {
            digitalWrite(ledPin, LOW);
            Serial.println("Still too long: LED OFF");
        }
    }
}
