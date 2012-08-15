// Definitions and function for running the expansion port.
//
//   Copyright (C) 2009 Robin Green
//
//   This file is part of the Midifighter Firmware.
//
//   The Midifighter Firmware is free software: you can redistribute it
//   and/or modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation, either version 3 of the
//   License, or (at your option) any later version.
//
//   The Midifighter Firmware is distributed in the hope that it will be
//   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   General Public License for more details.
//
//   You should have received a copy of the GNU General Public License along
//   with the Midifighter Firmware.  If not, see
//   <http://www.gnu.org/licenses/>.
//
// rgreen 2009-11-24

#include <avr/io.h>
#include <avr/interrupt.h>

#include "modeldefs.h"  // NOTE: include this first.

#include "spi.h"
#include "constants.h"
#include "expansion.h"
#include "key.h"

// Global values -------------------------------------------------------------

//uint8_t g_exp_digital_read;   // 4-bits of "enabled" flags, one for each pin.
//uint8_t g_exp_analog_read;    // 4-bits of "enabled" flags, one for each pin.

// Key states for the expansion port inputs.
uint8_t g_exp_key_debounce_buffer[DEBOUNCE_BUFFER_SIZE];
uint8_t g_exp_key_state;
uint8_t g_exp_key_prev_state;
uint8_t g_exp_key_down;
uint8_t g_exp_key_up;

// Array of previous ADC values for the analog reads, each one the full
// 10-bit range so we can track the lower bits and add hysteresis into
// the value changes.
uint16_t g_exp_analog_prev[NUM_ANALOG];


// Functions -----------------------------------------------------------------

// Setup the expansion port for reading.
void exp_setup()
{
    // NOTE: This function assumes that SPI has already been setup.

    // Setup Digital
    // -------------
    //
    // Set the digital expansion pins to outputs.
    // NOTE: The directions for EXP_KEY_BIT and EXP_LED_BIT needs to be
    // decided at use time.
    DDRD |= EXP_KEY_LATCH + EXP_LED_LATCH + EXP_KEY_CLOCK;
    // Clear the debounce buffer.
    for (uint8_t i=0; i<DEBOUNCE_BUFFER_SIZE; ++i) {
        g_exp_key_debounce_buffer[i] = 0;
    }

    // Setup ADC
    // ---------
    //
    // Turn on the select pins for output to the SPI devices
    DDRB |= ADC_SELECT;
    // Start with the ADC chip disabled (pin high)
    PORTB |= ADC_SELECT;
    // Read the four analog ports to generate an initial set of "previous"
    // values that we will be testing against.
    for (uint8_t i=0; i<NUM_ANALOG; ++i) {
        g_exp_analog_prev[i] = exp_adc_read(i);
    }

}

// This function is designed to be used inside the key-read interrupt
// service routine, so it has to be as fast as possible and make no
// assumptions about the state of any hardware it uses.
//
void exp_buffer_digital_inputs(void)
{
    // Where to write the next value in the ring buffer.
    static uint8_t ext_buffer_pos = 0;

    // Read the expansion port keys from a 74HC165 shift reg.
    //
    // set the EXP_KEY_BIT to an input.
    DDRD &= ~EXP_KEY_BIT;

    // latch the key values with a falling edge
    PORTD &= ~EXP_KEY_LATCH;
    PORTD |= EXP_KEY_LATCH;
    uint8_t value = 0;
    uint8_t bit = 0x80;

    // Clear the clock.
    PORTD &= ~EXP_KEY_CLOCK;

    for (uint8_t i=0; i<8; ++i) {
        // clock on a rising edge.
        PORTD |= EXP_KEY_CLOCK;
        // read a bit.
        if (PIND & EXP_KEY_BIT) {
            value &= ~bit;  // clear a bit
        } else {
            value |= bit;   // set a bit
        }
        // clear the clock.
        PORTD &= ~EXP_KEY_CLOCK;
        bit >>= 1;
    }

#ifdef REORDER_EXT_KEYS
    // Reorder bits | 3 | 1 | into  | 1 | 2 |
    //              | 4 | 2 |       | 3 | 4 |
    uint8_t temp = 0;
    temp |= value & 0x1 ? 0x02 : 0;
    temp |= value & 0x2 ? 0x08 : 0;
    temp |= value & 0x4 ? 0x01 : 0;
    temp |= value & 0x8 ? 0x04 : 0;
    value = temp;
#endif

    g_exp_key_debounce_buffer[ext_buffer_pos] = value;
    ext_buffer_pos = (ext_buffer_pos + 1) % DEBOUNCE_BUFFER_SIZE;
}

// Generate a debounced read of the digital input ports. For more on how
// this works see comments in "key.c"
//
uint8_t exp_key_read(void)
{
    g_exp_key_state = 0xff;
    for(uint8_t i=0; i<DEBOUNCE_BUFFER_SIZE; ++i) {
        g_exp_key_state &= g_exp_key_debounce_buffer[i];
    }
    return g_exp_key_state;
}

// Calculate the keydown and keyup states for the digital inputs.
//
void exp_key_calc(void)
{
    // Something is different, and it's set in the keystate.
    g_exp_key_down = (g_exp_key_prev_state ^ g_exp_key_state) &
                     g_exp_key_state;
    // Something is different, and it used to be set in the prev state.
    g_exp_key_up = (g_exp_key_prev_state ^ g_exp_key_state) &
                   g_exp_key_prev_state;
    g_exp_key_prev_state = g_exp_key_state;
}


// Analog ---------------------------------------------------------------------

// Get the 10-bit value from one of the four analog channels.
// The SPI protocol consists of sending and receiving three bytes:
//
//             S E N D            R E A D
//  byte1  7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
//         . . . . . . . 1    . . . . . . . .
//
//  byte2  7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
//         S . A B . . . .    . . . . . 0 a b
//
//  byte3  7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
//         . . . . . . . .    c d e f g h i j
//
//  S   = select single ended (1) or differential (0) measurement.
//  AB  = the port number, four ports numbered from 0b00 to 0b11.
//  a-j = the 10 bit ADC value, note the leading zero.
//  .   = don't care, do not use, could be anything.
//
uint16_t exp_adc_read(uint8_t channel)
{
    // Disable the LED controller by making sure it's latch is low. The LED
    // and ADC chips share the same SPI data and clock lines, so it's
    // essential to make sure the LED driver is not listening to the data
    // flowing down the SPI bus.
    PORTB &= ~LED_LATCH;

    // single channel reads only (no differential) and we select the ADC
    // channel here.
    uint8_t byte2 = 0b10000000 | ((channel & 0x03) << 4);

    // Enable the ADC chip by bringing the select line low.
    PORTB &= ~ADC_SELECT;
    // First byte wakes up the chip.
    spi_transmit(0b00000001);
    // Second byte sets up single-channel-read the channel.
    uint8_t topbyte = spi_transmit(byte2);
    // Third byte shifts in the remaining values.
    uint8_t lowbyte = spi_transmit(0b00000000);
    // Put the ADC chip back into hibernation by pulling the select pin high.
    PORTB |= ADC_SELECT;
    // Mask out the "don't care" bits and return the 10-bit value including
    // the leading zero at bit 11.
    return ((topbyte & 0x07) << 8) | lowbyte;
}

// ---------------------------------------------------------------------------

void exp_set_key_led(uint8_t state)
{
    // Turn off interrupts because the external LEDs and the external Key
    // Read lines share the same clock lines. An interrupt in the middle of
    // this sequence will give you corrupt LEDs.
    cli();

	// The only device configuration which uses reordering of the external key
	// is Serato, if rotate is enabled then we dont want to reorder the keys
if(!g_rotate_enable)
{
	
#ifdef REORDER_EXT_KEYS
    // Reorder bits | 1 | 2 | into  | 3 | 1 |
    //              | 3 | 4 |       | 4 | 2 |
    uint8_t temp = 0;
    temp |= state & 0x1 ? 0x04 : 0;
    temp |= state & 0x2 ? 0x01 : 0;
    temp |= state & 0x4 ? 0x08 : 0;
    temp |= state & 0x8 ? 0x02 : 0;
    state = temp;
#endif
}


    // set the EXP_LED_BIT to an output, no pullup.
    DDRD |= EXP_LED_BIT;

    // Start with the latch low.
    PORTD &= ~EXP_LED_LATCH;
    // Start with the clock high.
    PORTD |= EXP_LED_CLOCK;

    // loop over external keys setting their LEDs.
    uint8_t bit = 0x80;
    for (uint8_t i=0; i<8; ++i) {
        // set an LED
        if (state & bit) {
            PORTD |= EXP_LED_BIT;
        } else {
            PORTD &= ~EXP_LED_BIT;
        }

        // clock the bit into the LED controller on a falling edge.
        PORTD &= ~EXP_LED_CLOCK;
        PORTD |= EXP_LED_CLOCK;

        // next bit
        bit >>= 1;
    }

    // latch the result with a rising edge
    PORTD |= EXP_LED_LATCH;
    // leave the latch low
    PORTD &= ~EXP_LED_LATCH;

    // Turn interrupts back on now we're leaving the critical section.
    sei();
}

// ----------------------------------------------------------------------------
