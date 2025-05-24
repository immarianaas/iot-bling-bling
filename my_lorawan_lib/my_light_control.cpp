#include <my_light_control.h>
// #define SDA_PIN 6
// #define SCL_PIN 7

#define SDA_PIN 9
#define SCL_PIN 2

#define LED 8
#define PHOTO 4
#define BUTTON 5
#define ACCEL_INTR 19 // dunno yet

#define BUTTON_DEBOUNCE_TIME 500
#define STILL_DELAY_LIGHT_OFF 30000 // 30 seconds before light turns off

volatile bool accelDetected = false; // used in interrupts (static functions)
volatile bool buttonPressed = false; // used in interrupts (static functions)

My_LightControl::My_LightControl() : ADXL345(&Wire, 0x53)
{
}
void My_LightControl::init()
{
    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(ACCEL_INTR, INPUT_PULLUP);
    digitalWrite(LED, HIGH);

    Wire.setPins(SDA_PIN, SCL_PIN);

    attachInterrupt(digitalPinToInterrupt(ACCEL_INTR), accelISR, RISING);
    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, FALLING);
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
    ADXL345.setActivityThreshold(30); // Value from 0–255 (~62.5mg per LSB at ±2g)
    ADXL345.setInactivityThreshold(10);
    ADXL345.setTimeInactivity(5); // 5 seconds
    ADXL345.setRangeSetting(2);   // ±2g range
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
    if (buttonPressed && (currentTime - lastManualToggle > BUTTON_DEBOUNCE_TIME))
    {
        manual = !manual;
        isActive = manual;
        digitalWrite(LED, manual ? HIGH : LOW);
        lastManualToggle = currentTime;
        buttonPressed = false;
        Serial.print("Manual mode: ");
        Serial.println(manual);
    }
}

bool My_LightControl::is_accel_activity()
{
    if (!accelDetected)
        return false;

    Serial.println("accelDetected");
    byte interrupts = ADXL345.getInterruptSource();
    accelDetected = false;
    return ADXL345.triggered(interrupts, ADXL345_ACTIVITY);
}

bool My_LightControl::is_dark()
{
    int photo_val = analogRead(PHOTO);
    // Serial.println(photo_val);
    return photo_val < 30;
}

bool My_LightControl::is_active()
{
    return isActive;
}

void My_LightControl::light_ctrl_active()
{

    button();
    if (manual)
        return;

    bool accel = is_accel_activity();
    if (accel)
        Serial.println("accel detected");

    if (accel && is_dark())
    {
        isActive = true;
        digitalWrite(LED, HIGH);
        lastMoveTime = millis();
        Serial.println("Motion and dark: LED ON");
    }
    else if (millis() - lastMoveTime > STILL_DELAY_LIGHT_OFF)
    {
        isActive = false;
        digitalWrite(LED, LOW);
    }
}
