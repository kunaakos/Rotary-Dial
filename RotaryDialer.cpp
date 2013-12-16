#include "RotaryDialer.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NO_NUMBER   -1

// Require DEBOUNCE_DELAY milliseconds between state changes.
#define DEBOUNCE_DELAY  15

RotaryDialer::RotaryDialer(int _readyPinNr, int _pulsePinNr) : readyPinNr(_readyPinNr), pulsePinNr(_pulsePinNr), dialedNumberReady(false), state(WAITING)
{
    pinMode(readyPinNr, INPUT);
    pinMode(pulsePinNr, INPUT);
    digitalWrite(readyPinNr, HIGH);
    digitalWrite(pulsePinNr, HIGH);
    lastInputChangeMillis = millis();
}

bool RotaryDialer::readAndDebounce() {
    bool readyPinStatusOld = readyPinStatus;
    bool pulsePinStatusOld = pulsePinStatus;
    readyPinStatus = digitalRead(readyPinNr);
    pulsePinStatus = digitalRead(pulsePinNr);

    if (readyPinStatus != readyPinStatusOld || pulsePinStatus != pulsePinStatusOld)
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
    dialedNumberReady = false;
    dialedNumber = 0;
}

void RotaryDialer::completeDial() {
    if (dialedNumber > 0 && dialedNumber <= 10)
    {
        if (dialedNumber == 10) 
        {
            dialedNumber = 0;
        }
        dialedNumberReady = true;
    }
}

bool RotaryDialer::update() {
    if (readAndDebounce())
    {
        switch(state) {
            case WAITING:
                if (readyPinStatus == LOW && pulsePinStatus == LOW)
                {
                    changeToState(LOWPULSE);
                    startDial();
                }
                break;
            case LOWPULSE:
                if (readyPinStatus == HIGH && pulsePinStatus == HIGH)
                {
                    changeToState(HIGHPULSE);
                    dialedNumber++;
                }
                else if (readyPinStatus == HIGH && pulsePinStatus == LOW)
                {
                    completeDial();
                    changeToState(WAITING);
                }
                break;
            case HIGHPULSE:
                if (readyPinStatus == LOW && pulsePinStatus == LOW)
                {
                    changeToState(LOWPULSE);
                }
                break;
        }
    }
    return dialedNumberReady;
}

int RotaryDialer::getLastNumber() {
    if (dialedNumberReady)
    {
        dialedNumberReady = false;
        return dialedNumber;
    }
    else
    {
        return NO_NUMBER;
    }
}