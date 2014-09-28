//
//  main.c
//  TinyPWMTone
//
//  Created by Michael Kwasnicki on 11.09.14.
//  Copyright (c) 2014 Kwasi-ich. All rights reserved.
//

#ifndef F_CPU
#define F_CPU 1200000UL // ATTiny13A has 1,2MHz ( 9,6MHz with CKDIV8 )
#endif

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <iso646.h>
#include <stdbool.h>

#define SPEAKER_PIN     PB0
#define INT_PIN         PB1
#define LED1_PIN        PB3
#define LED2_PIN        PB4
#define INTERRUPTOR     PCINT1

#define _BV2( A, B ) ( _BV( A ) bitor _BV( B ) )
#define _BV3( A, B, C ) ( _BV2( A, B ) bitor _BV( C ) )
#define BIT_SET( PORT, BIT_FIELD ) PORT |=  BIT_FIELD;
#define BIT_CLR( PORT, BIT_FIELD ) PORT &= ~BIT_FIELD;
#define BIT_TGL( PORT, BIT_FIELD ) PORT ^=  BIT_FIELD;

#define BASE_DURATION   96

static const uint16_t melody1[] PROGMEM = {
    0x38c4, 0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4, 0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4, 0xbe84,
    0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4, 0xbe84, 0x9784, 0x7184, 0x9784, 0x4b84, 0x4b84, 0x9784, 0xbe84, 0x4b82,
    0x7182, 0x9784, 0xbe84, 0x38c4, 0xbe84, 0x4b84, 0x4784, 0x4b84, 0x5484, 0x4b84, 0x5481, 0x5f81, 0x6481, 0x7184,
    0x7184, 0x8e84, 0x7184, 0x6482, 0x7182, 0x7184, 0x8e84, 0x2ac4, 0x8e84, 0x7184, 0x7184, 0x5484, 0x4b84, 0x4784,
    0x23c2, 0x8e82, 0x7182, 0x8e82, 0x5f82, 0x7182, 0x7182, 0x8e82, 0x7184, 0x6484, 0xfe82, 0x7f82, 0x6482, 0x7f82,
    0x7182, 0x5481, 0x7181, 0x6482, 0x9782, 0x9782, 0x7182, 0x38c4, 0x5f84, 0x4b84, 0x3884, 0x9784, 0xbe84, 0x38c4,
    0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4, 0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4, 0xbe84, 0x9784,
    0x7184, 0x9784, 0xbe84, 0x38c4, 0xe382, 0x9782, 0x7182, 0x5f82, 0x4b88, 0xfe84, 0xa984, 0x6484, 0x23c4, 0xbe84,
    0x7184, 0x38c4, 0xe382, 0x9782, 0x7182, 0x5f82, 0x4b88, 0xfe84, 0xa984, 0x6484, 0x23c4, 0xbe84, 0x7184, 0x38c4,
    0xe382, 0x9782, 0x7182, 0x5f82, 0x4b88, 0xfe84, 0x5482, 0x6482, 0x7f84, 0x23c4, 0x5f82, 0x7182, 0x8e84, 0x2fc4,
    0x5f82, 0x7f82, 0x9784, 0x25c4, 0x6482, 0x7882, 0xa984, 0x38c4, 0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4,
    0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c4, 0xbe84, 0x9784, 0x7184, 0x9784, 0xbe84, 0x38c8
};

static const uint16_t melody2[] PROGMEM = {
    0x6488, 0x6a82, 0x6a88, 0x6482, 0x6488, 0x6a82, 0x6a88, 0x0002, 0x6488, 0x6a82, 0x6a88, 0x6482, 0x6484, 0x27c4,
    0x21c4, 0xf084, 0x4f82, 0x4f82, 0x4f82, 0x4f82, 0x4f82, 0x4f82, 0x4f82, 0x6482, 0x6482, 0x6488, 0x6482, 0x6482,
    0x5984, 0x4f84, 0x4f84, 0x5984, 0x5984, 0x27c4, 0x21c4, 0xf084, 0x4f82, 0x4f82, 0x4f82, 0x4f82, 0x4f82, 0x4f82,
    0x4f82, 0x6482, 0x6482, 0x6488, 0x6482, 0x6482, 0x5984, 0x6484, 0x6484, 0x6a84, 0x7884, 0x27c4, 0x21c4, 0xf084,
    0x3582, 0x3582, 0x3582, 0x3582, 0x3582, 0x3282, 0x3284, 0x3b82, 0x3b84, 0x4384, 0x4384, 0x3584, 0x3584, 0x3584,
    0x3b82, 0x3b84, 0x27c4, 0x21c4, 0xf084, 0x3582, 0x3582, 0x3582, 0x3582, 0x3582, 0x3282, 0x3284, 0x3b82, 0x3b84,
    0x4384, 0x4384, 0x3584, 0x3584, 0x3584, 0x3b82, 0x3b84, 0x4f84, 0x4f84, 0x5984, 0x4f88, 0x6488, 0x4f84, 0x4b84,
    0x4f84, 0x5984, 0x4f88, 0x6488, 0x4f84, 0x4b84, 0x4f84, 0x5984, 0x5988, 0x5988, 0x5984, 0x5984, 0x5984, 0x6484,
    0x4f88, 0x4f88, 0x4f84, 0x4f84, 0x4f84, 0x5984, 0x4f88, 0x6488, 0x4f84, 0x4b84, 0x4f84, 0x5984, 0x4f88, 0x6488,
    0x4f84, 0x4b84, 0x4f84, 0x5984, 0x5988, 0x5988, 0x5984, 0x5984, 0x5984, 0x6484, 0x4f88, 0x4f88, 0x4f88, 0x5988,
    0x7890
};



static void setup_power_down( void ) {
    BIT_SET( PORTB, _BV( INT_PIN ) );       // interrupt pin hi
    BIT_SET( PCMSK, _BV( INTERRUPTOR ) );   // set PB1 to trigger interrupt
    
    // Set Sleep Mode to Power-Down
    BIT_CLR( MCUCR, _BV( SM0 ) );
    BIT_SET( MCUCR, _BV( SM1 ) );
}



static void power_down( void ) {
    BIT_SET( GIFR, _BV( PCIF ) );   // Clear interrupt signal
    BIT_SET( GIMSK, _BV( PCIE ) );  // Pin Change Interrupt Enabled
    BIT_SET( MCUCR, _BV( SE ) );    // Enable Sleep Mode
    sei();                          // Enable Interrupts
    sleep_cpu();                    // Enter sleep mode
    cli();                          // Disable Interrupts
    BIT_CLR( MCUCR, _BV( SE ) );    // Disable Sleep Mode
    BIT_CLR( GIMSK, _BV( PCIE ) );  // Pin Change Interrupt Disabled
}



ISR( PCINT0_vect ) {
    // do nothing
}



int main( void ) {
    uint16_t tone;
    uint8_t freq;
    uint8_t prescaler;
    uint8_t duration;
    uint8_t i;
    const uint8_t n1 = sizeof( melody1 ) / sizeof( uint16_t );
    const uint8_t n2 = sizeof( melody2 ) / sizeof( uint16_t );
    
    // set pins as output
    DDRB = _BV3( SPEAKER_PIN, LED1_PIN, LED2_PIN );
    
    // CTC on
    TCCR0A = _BV2( WGM01, COM0A0 ); // CTC Mode, toggle OC0A on compare Match
    
    setup_power_down();
    
    // blinking lights to indicate device is powered up
    _delay_ms( 500 );
    BIT_SET( PORTB, _BV( LED1_PIN ) ); // LED on
    BIT_CLR( PORTB, _BV( LED2_PIN ) ); // LED off
    _delay_ms( 500 );
    BIT_TGL( PORTB, _BV2( LED1_PIN, LED2_PIN ) ); // LEDs toggle
    _delay_ms( 500 );
    BIT_CLR( PORTB, _BV2( LED1_PIN, LED2_PIN ) ); // LEDs off
    
	while ( true ) {
        power_down();
        
        BIT_SET( PORTB, _BV( LED1_PIN ) ); // LED on
        BIT_CLR( PORTB, _BV( LED2_PIN ) ); // LED off

        for ( i = 0; i < n1; i++ ) {
            tone = pgm_read_word( &melody1[i] );
            freq = ( tone >> 8 );
            prescaler = ( tone >> 6 ) bitand 0x3;
            duration = tone bitand 0x3F;
            OCR0A = freq;
            
            BIT_TGL( PORTB, _BV2( LED1_PIN, LED2_PIN ) ); // LEDs toggle
            
            TCCR0B = prescaler bitand 0x7;  // set the 3 LSBs to select clock prescaler
            
            do {
                _delay_ms( BASE_DURATION );
                duration--;
            } while ( duration );
            
            TCCR0B = 0; // stop clock
            _delay_ms( 10 );
        }
        
        BIT_CLR( PORTB, _BV2( LED1_PIN, LED2_PIN ) ); // LEDs off
        
        power_down();
        
        BIT_SET( PORTB, _BV( LED1_PIN ) ); // LED on
        BIT_CLR( PORTB, _BV( LED2_PIN ) ); // LED off

        for ( i = 0; i < n2; i++ ) {
            tone = pgm_read_word( &melody2[i] );
            freq = ( tone >> 8 );
            prescaler = ( tone >> 6 ) bitand 0x3;
            duration = tone bitand 0x3F;
            OCR0A = freq;
            
            BIT_TGL( PORTB, _BV2( LED1_PIN, LED2_PIN ) ); // LEDs toggle
            
            TCCR0B = prescaler bitand 0x7;  // set the 3 LSBs to select clock prescaler
            
            do {
                _delay_ms( BASE_DURATION );
                duration--;
            } while ( duration );
            
            TCCR0B = 0; // stop clock
            _delay_ms( 10 );
        }
        
        BIT_CLR( PORTB, _BV2( LED1_PIN, LED2_PIN ) ); // LEDs off
	}
}
