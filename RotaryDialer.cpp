#include "RotaryDialer.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NO_NUMBER   -1

// Require DEBOUNCE_DELAY milliseconds between state changes.
#define DEBOUNCE_DELAY  15

RotaryDialer::RotaryDialer(int _pinReady, int _pinPulse) : pinReady(_pinReady), pinPulse(_pinPulse), hasCompletedNumber(false), state(WAITING)
{
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

bool RotaryDialer::changeToState(enum State newState) {
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
                if (readyStatus == LOW && pulseStatus == LOW)
                {
                    changeToState(LOWPULSE);
                    startDial();
                }
                break;
            case LOWPULSE:
                if (readyStatus == HIGH && pulseStatus == HIGH)
                {
                    changeToState(HIGHPULSE);
                    number++;
                }
                else if (readyStatus == HIGH && pulseStatus == LOW)
                {
                    completeDial();
                    changeToState(WAITING);
                }
                break;
            case HIGHPULSE:
                if (readyStatus == LOW && pulseStatus == LOW)
                {
                    changeToState(LOWPULSE);
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