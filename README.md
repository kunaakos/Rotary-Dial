Rotary Dial MM
==============

Arduino library to read numbers from a rotary dial (or otherwise using pulse dialing; see [Wikipedia: Rotary dial](http://en.wikipedia.org/wiki/Rotary_dial)).

![front of a rotary telephone dial](http://i.imgur.com/tUDKwaX.jpg?1 "Pretty Lady optional")

This is based on the [Rotary-Dial](https://github.com/markfickett/Rotary-Dial) library, modified for the rotary dial used in Hungarian "Mechanikai Művek" phones (and possibly other Eastern European dials). It's not a drop-in replacement for the original version.

Connection & Circuit
--------------------

On the back of the dial are two connections relevant for this implementation.  One (the 'ready' switch) is normally open (NO). The other (the 'pulse' switch) is normally closed (NC), and is opened briefly for each pulse (roughly 10 - 20 Hz).

The expected circuit is:

	 ready switch (NO) connected to Arduino readyPin
	 pulse switch (NC) connected to Arduino pulsePin
	 GND

The expected sequence is:

	readyPin  pulsePin  state
	HIGH      LOW       default (waiting)
	LOW       LOW       ready to dial / for first pulse
	HIGH      HIGH      pulse received (number = 1)
	LOW       LOW       ready for next pulse
	HIGH      HIGH      pulse received (number = 2)
	LOW       ...       (repeat)
	HIGH      LOW       rotation complete, count recorded

There is 15ms allowed for debounce, which is the implementation's only
constraint on pulse speed.