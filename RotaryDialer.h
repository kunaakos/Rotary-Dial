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
		unsigned long lastStateChangeMillis;
		enum State {
			WAITING,
			LOWPULSE,
			HIGHPULSE
		};
		enum State state;

		/**
		 * Change state
		 */
		bool changeState(enum State newState);
		
		/**
		 * Check if enough time has elapsed since
		 * the last state change (to protect from noise).
		 */
		bool ifDebounced();

		/**
		 * 
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
		 *	which is closed (LOW) during dialing
		 * @param pulsePin connected to a NC (LOW) switch on the rotor
		 *	which is opened (HIGH) during each pulse
		 */
		RotaryDialer(int readyPin, int pulsePin);

		/**
		 * Initialize the pins; digital read pins, held HIGH.
		 */
		void setup();

		/**
		 * Check the pins and update state (in or out of a pulse,
		 * dialing complete, etc). This must be called at least as
		 * pulses; assuming 10 pulses per second, every 50ms.
		 */
		bool update();

		/**
		 * Get the most recently dialed number. After this is called,
		 * hasNextNumber will return false until a new number is dialed.
		 * @return the most recently dialed number, and clear
		 *	hasNextNumber
		 */
		int getLastNumber();
};

