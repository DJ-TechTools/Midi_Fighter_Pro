// Mathematical functions for Midifighter platform
//
//   Copyright (C) 2010-2011 Robin Green
//
// rgreen 2010-11-10

#ifndef _RANDOM_H_INCLUDED
#define _RANDOM_H_INCLUDED

#include <stdint.h>

// Macros and inline ----------------------------------------------------------

#define MIN(a, b) (a)<=(b)?(a):(b)
#define MAX(a, b) (a)>=(b)?(a):(b)

// Constants ------------------------------------------------------------------

extern const uint16_t gamma16_table[256];
extern const uint8_t gamma8_table[256];

// Functions ------------------------------------------------------------------

void set_seed16(uint16_t seed);
uint16_t random16(void);
void set_seed32(uint32_t seed);
uint32_t random32(void);
int8_t sintable(uint8_t angle);
uint8_t lerp(uint8_t high, uint8_t low, uint8_t t);
int8_t clamp(int8_t value, int8_t low, int8_t high);
float clampf(float value, float low, float high);
uint8_t smoothstep(uint8_t min, uint8_t max, uint8_t t);
uint32_t random_color(void);
uint16_t rightmost_bit_16(uint16_t value);
uint16_t rotate16_right(const uint16_t value);
uint16_t rotate16_left(const uint16_t value);

// ----------------------------------------------------------------------------

#endif // _RANDOM_H_INCLUDED
