#if 0

#include "Arduino.h"

Servo servo;

int servoPin = D0;
int lightPin = D1;
int buttonPin = D2;
int openSwitchPin = D3;
int closeSwitchPin = D4;
int faultPin = D5;

int pos = 20;
// bool opening = false;
// bool closing = false;




/**
 * Setup the door hardware (all I/O should be configured here)
 * 
 * This routine should be called only once from setup()
 */
void setupHardware() {
    Serial.begin(9600);
    servo.attach(servoPin);
    servo.write(20);
    pinMode(lightPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(openSwitchPin, INPUT_PULLUP);
    pinMode(closeSwitchPin, INPUT_PULLUP);
    pinMode(faultPin, INPUT_PULLUP);
}

/**
 * Return true if the door open/close button is pressed
 * 
 * Note: this is directly based on hardware.  No debouncing or
 *       other processing is performed.
 * 
 * return  true if buttons is currently pressed, false otherwise
 */
bool isButtonPressed() {
    if (digitalRead(buttonPin) == 0) {
        return true;
    }
    return false;
}

/**
 * Return true if the door is fully closed
 * 
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 * 
 * return  true if the door is completely closed, false otherwise
 */
bool isDoorFullyClosed() {
    if (digitalRead(closeSwitchPin) == 0) {
        return true;
    }
    return false;
}

/**
 * Return true if the door has experienced a fault
 * 
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 * 
 * return  true if the door is has experienced a fault
 */
bool isFaultActive() {
    if (digitalRead(faultPin) == 0) {
        return true;
    }
    return false;
}

/**
 * Return true if the door is fully open
 * 
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 * 
 * return  true if the door is completely open, false otherwise
 */
bool isDoorFullyOpen() {
    if (digitalRead(openSwitchPin) == 0) {
        return true;
    }
    return false;
}

/**
 * This function will start the motor moving in a direction that opens the door.
 * 
 * Note: This is a non-blocking function.  It will return immediately
 *       and the motor will continue to opperate until stopped or reversed.
 * 
 * return void 
 */
void startMotorOpening() {
    return;
}

/**
 * This function will start the motor moving in a direction closes the door.
 * 
 * Note: This is a non-blocking function.  It will return immediately
 *       and the motor will continue to opperate until stopped or reversed.
 * 
 * return void 
 */
void startMotorClosing() {
    return;
}

/**
 * This function will stop all motor movement.
 * 
 * Note: This is a non-blocking function.  It will return immediately.
 * 
 * return void 
 */
void stopMotor() {
    //currentState = PAU;
    return;
}

/**
 * This function will control the state of the light on the opener.
 * 
 * Parameter: on: true indicates the light should enter the "on" state; 
 *                false indicates the light should enter the "off" state
 * 
 * Note: This is a non-blocking function.  It will return immediately.
 * 
 * return void 
 */
void setLight(boolean on) {
    if (on) {
        digitalWrite(lightPin, HIGH);
    } else {
        digitalWrite(lightPin, LOW);
    }
    return;
}

#endif