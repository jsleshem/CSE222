Servo servo;

int servoPin = D0;
int lightPin = D1;
int buttonPin = D2;
int openSwitchPin = D3;
int closeSwitchPin = D4;
int faultPin = D5;

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

int pos = 20;
// bool opening = false;
// bool closing = false;

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

int cloudButtonPress(String nothing) {
    if (currentState == OPENING){
        changeState("PAUSED");
    }
    else if (currentState == OPENED) {
        changeState("CLOSING");
    }
    else if (currentState == CLOSING){
        changeState("PAUSED");
    }
    else if (currentState == CLOSED) {
        changeState("OPENING");
    }
    else if (currentState == PAUSED){
        if (prevState == OPENING){
            changeState("CLOSING");
        }
        if (prevState == CLOSING){
            changeState("OPENING");
        }
    }
    return 0;
}

int whichState(String nothing) {
    return currentState;
}


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
    Particle.function("buttonPress", cloudButtonPress);
    Particle.function("whichState", whichState);
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
    changeState("OPENING");
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
    changeState("CLOSING");
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

void printState() {
    if (prevState != currentState) {
        Serial.println(currentState);
    }
}

void writePos(int pos) {
    servo.write(pos);
}

void autoOffBody() {
    setLight(false);
}

void setup() {
    setupHardware();
}

Timer autoOff(5000, autoOffBody, true);

void loop() {
    int now = millis();
    switch (currentState) {
        case OPENING:
        
            Serial.println("OPENING");
        
            setLight(true);
            if (now % 50 == 0){
                if (pos == 160){
                    pos = 20;
                } else {
                    pos+=5;
                }
                writePos(pos);
            }

            if (isDoorFullyOpen() != lastReadOpened){
                lastOpenedPressTime = millis();
            }
            if (isDoorFullyOpen() && now-lastOpenedPressTime > MIN_BUTTON_PRESS) {
                changeState("OPENED");
                autoOff.start();
            }
            lastReadOpened = isDoorFullyOpen();
            
            if (!isButtonPressed()) {
                pressed = false;
            }
            
            if (!pressed){
            
            if (isButtonPressed() != lastReadButtonToPause){
                lastButtonToPausePressTime = millis();
            }
            if (isButtonPressed() && now-lastButtonToPausePressTime > MIN_BUTTON_PRESS) {
                pressed = true;
                changeState("PAUSED");
                autoOff.start();
            }
            lastReadButtonToPause = isButtonPressed();
            
            }
        
        
            if (isFaultActive()){
                changeState("FAULTED");
                setLight(true);
            }
            
            break;
        case CLOSING:
        
            Serial.println("CLOSING");
        
            setLight(true);
            if (now % 50 == 0){
                if (pos == 20){
                    pos = 160;
                } else {
                    pos-=5;
                }
                writePos(pos);
            }
            
            if (isDoorFullyClosed() != lastReadClosed){
                lastClosedPressTime = millis();
            }
            if (isDoorFullyClosed() && now-lastClosedPressTime > MIN_BUTTON_PRESS) {
                changeState("CLOSED");
                autoOff.start();
            }
            lastReadClosed = isDoorFullyClosed();
            
            
            
            
            
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
                    autoOff.start();
                }
                lastReadButtonToPause = isButtonPressed();
            
            }
            
            
            
            
            
            
            if (isFaultActive()){
                currentState = FAULTED;
                prevState = CLOSING;
            }
        
            break;
            
        case OPENED:
            
            Serial.println("OPENED");
            
            if (isButtonPressed() != lastReadButtonToClose){
                lastButtonToClosePressTime = millis();
            }
            if (isButtonPressed() && now-lastButtonToClosePressTime > MIN_BUTTON_PRESS) {
                startMotorClosing();
                pressed = true;
            }
            lastReadButtonToClose = isButtonPressed();
            
        
            break;
            
        case CLOSED:
        
            Serial.println("CLOSED");
        
            if (isButtonPressed() != lastReadButtonToOpen){
                lastButtonToOpenPressTime = millis();
            }
            if (isButtonPressed() && now-lastButtonToOpenPressTime > MIN_BUTTON_PRESS) {
                startMotorOpening();
                pressed = true;
            }
            lastReadButtonToOpen = isButtonPressed();
            
        
            break;
        case PAUSED:
        
            Serial.println("PAUSED");
            
            
            
            
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
                    } else {
                        startMotorOpening();
                    }
                }
                lastReadButton = isButtonPressed();
            
            }
        
            break;
        case FAULTED:
            if (now % 500 == 0) {
                if (digitalRead(lightPin) == 1) {
                    setLight(false);
                } else {
                    setLight(true);
                }
            }
            
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
                }
                lastReadButton = isButtonPressed();
            
            }
            
            break;
    }
}












