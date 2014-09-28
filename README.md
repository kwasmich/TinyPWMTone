TinyPWMTone
===========

Port of PiPWMTone to the ATtiny13A micro controller

Is using the Timer/Counter in CTC mode to generate tones and the ATtiny13A's ability for a
power-down to conserve battery life.

Just press the button and one of the two programmed melodies will sound while the two LEDs are
changing on each tone.



Melody
------

`melody.c` is used to convert a melody from the PiPWMTone-Format into a series of hex codes which are directly
interpreted. Each 16-bit word combines the OCR0A code for the divisor, the prescaler in TCCR0B and the duration of a
tone. The tone frequency is according to ATtiny13A's documentation

``
f = f_{CLK} / ( 2 * N * ( 1 + OCR ) )
N := prescaler
OCR := divisor
``

This formula has implications on the accuracy of the given tone.
Higher frequencies are more inaccurate then lower ones.



Wiring
------

``
          GND  GND
           |    |
VCC       BTN  SPK
 |    |    |    |
-----------------
|               |
|   ATtiny13A   |
|               |
-----------------
 |    |    |   |
    LED2 LED1 GND
      |    |
     GND  GND
``
