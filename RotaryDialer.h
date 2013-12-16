#pragma once

/**
 * Read numbers from a rotary dial (or otherwise using pulse dialing; see
 * http://en.wikipedia.org/wiki/Rotary_dial ). 
 *
 * See the README for further documentation.
 */
class RotaryDialer {
    private:
        int pinReady;
        int pinPulse;
        bool hasCompletedNumber;
        int number;
        unsigned long lastInputChangeMillis;
        bool readyStatus;
        bool pulseStatus;
        enum State {
            WAITING,
            LOWPULSE,
            HIGHPULSE
        };
        enum State state;

        /**
         * Change state
         */
        bool changeToState(enum State newState);
        
        /**
         * read switches and debounce
         * @return true if debounced
         */
        bool readAndDebounce();

        /**
         * Check if enough time has elapsed since
         * the last state change (debounce).
         */
        bool ifDebounced();

        /**
         * To be called when the rotor leaves
         * its rest position; resets number to 0
         */
        void startDial();

        /**
         * To be called when when the rotor returns
         * to its rest position; save the number, if valid.
         */
        void completeDial();

    public:
        /**
         * Create a new RotaryDialer listening on the given pins.
         * @param readyPin connected to a NO (HIGH) switch on the rotor
         *  which is closed (LOW) during dialing and opened (HIGH) during each pulse
         * @param pulsePin connected to a NC (LOW) switch on the rotor
         *  which is opened (HIGH) during each pulse
         */
        RotaryDialer(int readyPin, int pulsePin);

        /**
         * Check the pins and update state (in or out of a pulse,
         * dialing complete, etc). This must be called at least as
         * pulses; assuming 10 pulses per second, every 50ms.
         * @return true if a new number was dialed, false otherwise
         */
        bool update();

        /**
         * Get the most recently dialed number. 
         * @return the most recently dialed number
         */
        int getLastNumber();
};