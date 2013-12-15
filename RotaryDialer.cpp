#include "RotaryDialer.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NO_NUMBER	-1

// Require DEBOUNCE_DELAY milliseconds between state changes. This value is
// noted in the class documentation.
#define DEBOUNCE_DELAY	15

RotaryDialer::RotaryDialer(int readyPin, int pulsePin) :
	pinReady(readyPin), pinPulse(pulsePin), hasCompletedNumber(false),
	state(WAITING)
{ }

void RotaryDialer::setup() {
	pinMode(pinReady, INPUT);
	pinMode(pinPulse, INPUT);
	digitalWrite(pinReady, HIGH);
	digitalWrite(pinPulse, HIGH);
	lastStateChangeMillis = millis();
}

bool RotaryDialer::ifDebounced() {
	unsigned long currentMillis = millis();
	if (currentMillis < lastStateChangeMillis) // clock wrapped; ignore (but could figure it out in this case)
	{
		lastStateChangeMillis = currentMillis;
		return false;
	} else if (currentMillis - lastStateChangeMillis > DEBOUNCE_DELAY) // OK to change state
	{
		return true;
	} else // not OK to change state
	{
		return false;
	}
}

bool RotaryDialer::changeState(enum State newState) {
	state = newState;
	lastStateChangeMillis = millis();
}

void RotaryDialer::startDial() {
  hasCompletedNumber = false;
	number = 0;
}

void RotaryDialer::completeDial() {
	if (number > 0 && number <= 10) {
		if (number == 10) {
			number = 0;
		}
		hasCompletedNumber = true;
	}
}

bool RotaryDialer::update() {
	int readyStatus = digitalRead(pinReady);
	int pulseStatus = digitalRead(pinPulse);

	switch(state) {
    
    case WAITING:
    	//Serial.println("WAIT");
      if (readyStatus == LOW && pulseStatus == LOW && ifDebounced())
      {
        changeState(LOWPULSE);
        startDial(); 
      }
      break;

    case LOWPULSE:
      //Serial.println("LOW ");
      if (readyStatus == HIGH && pulseStatus == HIGH && ifDebounced())
      {
        changeState(HIGHPULSE);
        number++;
        
      } else if (readyStatus == HIGH && pulseStatus == LOW && ifDebounced())
      {
        changeState(WAITING);
        completeDial(); 
      }
      break;

    case HIGHPULSE:
    	//Serial.println("HIGH");
      if (readyStatus == LOW && pulseStatus == LOW && ifDebounced())
      {
        changeState(LOWPULSE);
      } else if (readyStatus == HIGH && pulseStatus == LOW && ifDebounced())
      {
        changeState(WAITING); 
        completeDial();
      }
      break;
  }

	return hasCompletedNumber;
}

int RotaryDialer::getLastNumber() {
	if (hasCompletedNumber) 
	{
		hasCompletedNumber = false;
		return number;
	} else 
	{
		return NO_NUMBER;
	}
}

