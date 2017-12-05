#include "GarageHardware.h"

// Primary Photon ID: 27001f001951353337343731
// Remote Photon ID: 2f0049000c47343438323536
// Product ID: 6238
// Client ID: garage-door-app-2719
// Client Secret: 3d6660ac010f48513147d74f72e090240829c79e


bool lastReadOpened = true;
bool lastReadClosed = true;
bool lastReadButton = true;
bool lastReadButtonToClose = true;
bool lastReadButtonToOpen = true;
bool lastReadButtonToPause = true;
long lastButtonPressTime = 0;
long lastButtonToClosePressTime = 0;
long lastButtonToOpenPressTime = 0;
long lastButtonToPausePressTime = 0;
long lastOpenedPressTime = 0;
long lastClosedPressTime = 0;
int MIN_BUTTON_PRESS = 100;
bool pressed = false;
bool autoCloseState = false;

enum State {
    OPENED = 0,
    CLOSED = 1,
    OPENING = 2,
    CLOSING = 3,
    PAUSED = 4,
    FAULTED = 5
};

State currentState = CLOSED;
State prevState = CLOSED; //doesn't matter



void autoClose() {
    changeState("CLOSING");
    startMotorClosing();
}

Timer autoCloseTimer(5000, autoClose, true);

int autoCloseSet(String state) {
    if (state == "FALSE"){
        autoCloseState = false;
    } else {
        autoCloseTimer.changePeriod(atoi(state));
        autoCloseState = true;
        if (currentState == OPENED) {
            autoCloseTimer.start();
        }
    }
}



int changeState(String newState) {
    if (newState == "OPENING"){
        prevState = currentState;
        currentState = OPENING;
        Particle.publish("state-change", "2" , 0);
    }
    if (newState == "CLOSING"){
        prevState = currentState;
        currentState = CLOSING;
        Particle.publish("state-change", "3" , 0);
    }
    if (newState == "CLOSED"){
        prevState = currentState;
        currentState = CLOSED;
        Particle.publish("state-change", "1", 0);
    }
    if (newState == "OPENED"){
        if (autoCloseState == true) {
            autoCloseTimer.start();
        }
        prevState = currentState;
        currentState = OPENED;
        Particle.publish("state-change", "0", 0);
    }
    if (newState == "PAUSED"){
        prevState = currentState;
        currentState = PAUSED;
        Particle.publish("state-change", "4", 0);
    }
    if (newState == "FAULTED"){
        prevState = currentState;
        currentState = FAULTED;
        Particle.publish("state-change", "5", 0);
    }
    
    return 0;
}

void remotePress(const char *event, const char *data) {
    cloudButtonPress("");
}

int cloudButtonPress(String nothing) {
    if (currentState == OPENING){
        changeState("PAUSED");
        stopMotor();
    }
    else if (currentState == OPENED) {
        changeState("CLOSING");
        startMotorClosing();
    }
    else if (currentState == CLOSING){
        changeState("PAUSED");
        stopMotor();
    }
    else if (currentState == CLOSED) {
        changeState("OPENING");
        startMotorOpening();
    }
    else if (currentState == PAUSED){
        if (prevState == OPENING){
            changeState("CLOSING");
            startMotorClosing();
        }
        if (prevState == CLOSING){
            changeState("OPENING");
            startMotorOpening();
        }
    }
    else if (currentState == FAULTED) {
        changeState("OPENING");
        startMotorOpening();
    }
    return 0;
}

int whichState(String nothing) {
    return currentState;
}

void printState() {
    if (prevState != currentState) {
        Serial.println(currentState);
    }
}

// Legacy servo code

// void writePos(int pos) {
//     servo.write(pos);
// }

void setup() {
    setupHardware();
    Serial.begin(9600);
    if (isDoorFullyClosed()) {
        currentState = CLOSED;
    } else if (isDoorFullyOpen()) {
        currentState = OPENED;
    } else if (isFaultActive()) {
        currentState = FAULTED;
    } else {
        prevState = CLOSING;
        currentState = CLOSED;
    }
    Particle.subscribe("garagedoor/button", remotePress, MY_DEVICES);
    Particle.function("buttonPress", cloudButtonPress);
    Particle.function("whichState", whichState);
    Particle.function("autoCloseSet", autoCloseSet);
}

void autoOffBody() {
    setLight(false);
}

Timer autoOff(5000, autoOffBody, true);


void loop() {
    int now = millis();
    switch (currentState) {
        
        // OPENING CASE
        case OPENING:
        
            Serial.println("OPENING");
        
            setLight(true);
            
            // Legacy servo code
            // if (now % 50 == 0){
            //     if (pos == 160){
            //         pos = 20;
            //     } else {
            //         pos+=5;
            //     }
            //     writePos(pos);
            // }

            // Move to Opened
            if (isDoorFullyOpen() != lastReadOpened){
                lastOpenedPressTime = millis();
            }
            if (isDoorFullyOpen() && now-lastOpenedPressTime > MIN_BUTTON_PRESS) {
                changeState("OPENED");
                stopMotor();
                autoOff.start();
            }
            lastReadOpened = isDoorFullyOpen();
            
            if (!isButtonPressed()) {
                pressed = false;
            }
            
            
            // Move to Paused
            if (!pressed){
            
            if (isButtonPressed() != lastReadButtonToPause){
                lastButtonToPausePressTime = millis();
            }
            if (isButtonPressed() && now-lastButtonToPausePressTime > MIN_BUTTON_PRESS) {
                pressed = true;
                changeState("PAUSED");
                stopMotor();
                autoOff.start();
            }
            lastReadButtonToPause = isButtonPressed();
            
            }
        
        
            // Move to faulted
            if (isFaultActive()){
                changeState("FAULTED");
                stopMotor();
            }
            
            break;
            
        // CLOSING STATE
        case CLOSING:
        
            Serial.println("CLOSING");
        
            setLight(true);
            
            // Legacy servo code
            // if (now % 50 == 0){
            //     if (pos == 20){
            //         pos = 160;
            //     } else {
            //         pos-=5;
            //     }
            //     writePos(pos);
            // }
            
            // Move to Closed
            if (isDoorFullyClosed() != lastReadClosed){
                lastClosedPressTime = millis();
            }
            if (isDoorFullyClosed() && now-lastClosedPressTime > MIN_BUTTON_PRESS) {
                changeState("CLOSED");
                stopMotor();
                autoOff.start();
            }
            lastReadClosed = isDoorFullyClosed();
            
            
            
            
            // Move to Paused
            if (!isButtonPressed()) {
                pressed = false;
            }
            
            if (!pressed) {
            
                if (isButtonPressed() != lastReadButtonToPause){
                    lastButtonToPausePressTime = millis();
                }
                if (isButtonPressed() && now-lastButtonToPausePressTime > MIN_BUTTON_PRESS) {
                    pressed = true;
                    changeState("PAUSED");
                    stopMotor();
                    autoOff.start();
                }
                lastReadButtonToPause = isButtonPressed();
            
            }
            
            
            
            
            
            // Move to faulted
            if (isFaultActive()){
                changeState("FAULTED");
                stopMotor();
            }
        
            break;
        
        // OPENED CASE
        case OPENED:
            
            Serial.println("OPENED");
            
            
            // Move to Closing
            if (isButtonPressed() != lastReadButtonToClose){
                lastButtonToClosePressTime = millis();
            }
            if (isButtonPressed() && now-lastButtonToClosePressTime > MIN_BUTTON_PRESS) {
                startMotorClosing();
                changeState("CLOSING");
                pressed = true;
            }
            lastReadButtonToClose = isButtonPressed();
            
        
            break;
        
        // CLOSED CASE
        case CLOSED:
        
            Serial.println("CLOSED");
        
        
            // Move to Opening
            if (isButtonPressed() != lastReadButtonToOpen){
                lastButtonToOpenPressTime = millis();
            }
            if (isButtonPressed() && now-lastButtonToOpenPressTime > MIN_BUTTON_PRESS) {
                startMotorOpening();
                changeState("OPENING");
                pressed = true;
            }
            lastReadButtonToOpen = isButtonPressed();
            
        
            break;
            
        // PAUSED CASE
        case PAUSED:
        
            Serial.println("PAUSED");
            
            
            
            // Move to either Opening or Closing 
            if (!isButtonPressed()) {
                pressed = false;
            }
            
            if (!pressed) {
            
                if (isButtonPressed() != lastReadButton){
                    lastButtonPressTime = millis();
                }
                if (isButtonPressed() && now-lastButtonPressTime > MIN_BUTTON_PRESS) {
                    pressed = true;
                    if (prevState == OPENING){
                        startMotorClosing();
                        changeState("CLOSING");
                        
                    } else {
                        startMotorOpening();
                        changeState("OPENING");
                    }
                }
                lastReadButton = isButtonPressed();
            
            }
        
            break;
            
        // FAULT CASE
        case FAULTED:
        
            // flashes light
            if ((now % 500) < 250) {
                setLight(false);
            } else {
                setLight(true);
            }
            
            // Move to opening
            if (!isButtonPressed()) {
                pressed = false;
            }
            
            if (!pressed) {
            
                if (isButtonPressed() != lastReadButton){
                    lastButtonPressTime = millis();
                }
                if (isButtonPressed() && now-lastButtonPressTime > MIN_BUTTON_PRESS) {
                    pressed = true;
                    startMotorOpening();
                    changeState("OPENING");
                }
                lastReadButton = isButtonPressed();
            
            }
            
            break;
    }
}












