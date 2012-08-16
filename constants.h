// Constant values shared by all subsystems for DJTechTools Midifighter.
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
// rgreen 2009-05-02

#ifndef _CONSTANTS_H_INCLUDED
#define _CONSTANTS_H_INCLUDED

// Constant values -------------------------------------------------------------

#define DEBOUNCE_BUFFER_SIZE 10

#define SPI_MISO   _BV(PB3)  // SPI master in slave out
#define SPI_MOSI   _BV(PB2)  // SPI master out slave in
#define SPI_CLOCK  _BV(PB1)  // SPI clock pin
#define ADC_SELECT _BV(PB5)  // Turn on the ADC chip (active low)
#define LED_LATCH  _BV(PB0)  // Latch the current controller state to the LEDs
#define LED_BLANK  _BV(PB4)  // Turn off all LEDs

#define KEY_HIBIT  _BV(PC4)  // Key read input high bit
#define KEY_CLOCK  _BV(PC5)  // Key read clock pin
#define KEY_LOBIT  _BV(PC6)  // Key read input low bit
#define KEY_LATCH  _BV(PC7)  // Key latch state to shift-out registers

#define EXP_LED_BIT   _BV(PD5)  // digital port 4 (shared out)
#define EXP_KEY_BIT   _BV(PD5)  // digital port 4 (shared in)
#define EXP_KEY_LATCH _BV(PD4)  // digital port 3 out
#define EXP_LED_LATCH _BV(PD3)  // digital port 2 out
#define EXP_KEY_CLOCK _BV(PD2)  // digital port 1 (shared out)
#define EXP_LED_CLOCK _BV(PD2)  // digital port 1 (shared out)

// #define EXP_DIGITAL0 _BV(PD2)  // Expansion port digital pin 1
// #define EXP_DIGITAL1 _BV(PD3)  // Expansion port digital pin 2
// #define EXP_DIGITAL2 _BV(PD4)  // Expansion port digital pin 3
// #define EXP_DIGITAL3 _BV(PD5)  // Expansion port digital pin 4

// 14bit device family ID, LSB first
#define DEVICE_FAMILY   0x02, 0x00

// 14bit device model ID, LSB first
#if defined(TRAKTOR_H)
// Cuemaster
	#define DEVICE_MODEL    0x00, 0x00
	#define MF_PRO_CUEMASTER
#elif defined(TRAKTOR_V)
// Beatmasher
	#define DEVICE_MODEL    0x01, 0x00
	#define MF_PRO_BEATMASHER
#elif defined(SERATO)
// Super knobs
	#define DEVICE_MODEL    0x02, 0x00
	#define MF_PRO_SUPERKNOB
#endif

// Should be the date of this firmware release, in hex, in the following format: 0xYYYYMMDD
#define DEVICE_VERSION  0x20120816

#define EEPROM_VERSION  6  // Increment this when the eeprom layout requires
                           // resetting to the factory default.

// EEPROM memory locations of persistent settings
#define EE_EEPROM_VERSION      0x0000  // Is the EEPROM layout current?
#define EE_FIRST_BOOT_CHECK    0x0001  // Hardware passed mfr testing?
#define EE_MIDI_CHANNEL        0x0002  // MIDI channel byte (0..15)
#define EE_MIDI_VELOCITY       0x0003  // MIDI velocity byte (0..127)
#define EE_KEY_KEYPRESS_LED    0x0004  // Light the LED of pressed keys (bool)
#define EE_KEY_FOURBANKS       0x0005  // Multiple banks of keys (0..2)
#define EE_EXP_DIGITAL_ENABLED 0x0006  // Read from Digital pins (4-bits)
#define EE_EXP_ANALOG_ENABLED  0x0007  // Read from ADC pins (4-bits)
#define EE_AUTO_UPDATE         0x0008  // Autmatically keep firmware up to date (1-bit)
#define EE_DEVICE_MODE         0x0009  // Key grid rotation (2-bits)
#define EE_COMBOS_ENABLE       0x000a  // Combos enabled (1-bit)
#define EE_MULTIPLEXER_ENABLE  0x000b  // Multiplexer connected to A1 and D1,D2,D3 (1-bit)
#define EE_ROTATE_ENABLE       0x000c  // Enables device rotation 

// SysEx MIDI message manufacturer ID
#define MANUFACTURER_ID 0x0179

// Fourbanks modes
#define FOURBANKS_OFF 0
#define FOURBANKS_INTERNAL 1
#define FOURBANKS_EXTERNAL 2

// Device Mode changes MIDI messages to suit certain software
#define TRAKTOR   0                   //default
#define ABLETON   1
#define RESV      2
#define RESV      3

// Number of MIDI notes tracked
#define MIDI_MAX_NOTES 128

// Note number of the basenote for the keys
#define MIDI_BASE_NOTE 36

#endif // _CONSTANTS_H_INCLUDED
