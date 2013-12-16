#include "RotaryDialer.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NO_NUMBER   -1

// Require DEBOUNCE_DELAY milliseconds between state changes.
#define DEBOUNCE_DELAY  15

RotaryDialer::RotaryDialer(int readyPin, int pulsePin) :
    pinReady(readyPin), pinPulse(pulsePin), hasCompletedNumber(false),
    state(WAITING)
{

}

void RotaryDialer::setup() {
    pinMode(pinReady, INPUT);
    pinMode(pinPulse, INPUT);
    digitalWrite(pinReady, HIGH);
    digitalWrite(pinPulse, HIGH);
    lastInputChangeMillis = millis();
}

bool RotaryDialer::readAndDebounce() {
    bool readyStatusOld = readyStatus;
    bool pulseStatusOld = pulseStatus;
    readyStatus = digitalRead(pinReady);
    pulseStatus = digitalRead(pinPulse);

    if (readyStatus != readyStatusOld || pulseStatus !=pulseStatusOld)
    {
        lastInputChangeMillis = millis();
        return false;
    }
    else
    {
        return ifDebounced();
    }

}

bool RotaryDialer::ifDebounced() {
    unsigned long currentMillis = millis();
    if (currentMillis < lastInputChangeMillis) // clock wrapped; ignore (but could figure it out in this case)
    {
        lastInputChangeMillis = currentMillis;
        return false;
    }
    else if (currentMillis - lastInputChangeMillis > DEBOUNCE_DELAY) // OK to change state
    {
        return true;
    }
    else // not OK to change state
    {
        return false;
    }
}

bool RotaryDialer::changeState(enum State newState) {
    state = newState;
}

void RotaryDialer::startDial() {
    hasCompletedNumber = false;
    number = 0;
}

void RotaryDialer::completeDial() {
    if (number > 0 && number <= 10)
    {
        if (number == 10) 
        {
            number = 0;
        }
        hasCompletedNumber = true;
    }
}

bool RotaryDialer::update() {
    if (readAndDebounce())
    {
        switch(state) {
        
            case WAITING:
                //Serial.println("WAIT");
                if (readyStatus == LOW && pulseStatus == LOW)
                {
                    changeState(LOWPULSE);
                    startDial(); 
                }
                break;

            case LOWPULSE:
                //Serial.println("LOW ");
                if (readyStatus == HIGH && pulseStatus == HIGH)
                {
                    changeState(HIGHPULSE);
                    number++;
                }
                else if (readyStatus == HIGH && pulseStatus == LOW)
                {
                    completeDial();
                    changeState(WAITING);
                }
                break;

            case HIGHPULSE:
                //Serial.println("HIGH");
                if (readyStatus == LOW && pulseStatus == LOW)
                {
                    changeState(LOWPULSE);
                }
                break;
        }
    }   
    return hasCompletedNumber;
}

int RotaryDialer::getLastNumber() {
    if (hasCompletedNumber)
    {
        hasCompletedNumber = false;
        return number;
    }
    else
    {
        return NO_NUMBER;
    }
}