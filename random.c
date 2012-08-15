// Random number generator for Midifighter 30mm RGB
//
//   Copyright (C) 2010-2011 Robin Green
//
// rgreen 2010-11-10


#include <avr/io.h>
#include <avr/pgmspace.h>
#include "random.h"

// global data ----------------------------------------------------------------

// Sine and Gradient tables generated from Python using:
//
//    table = [int(math.sin(x * 2.0 * math.pi / 32) * 128) for x in range(0,33)]
//    grad = [ table[i+1] - table[i] for i in range(0,32)]
//
static const int8_t sin_table[32] PROGMEM = {
      0,   24,   48,   70,   89,  105,  117,  124,
    127,  124,  117,  105,   89,   70,   48,   24,
      0,  -24,  -48,  -70,  -89, -105, -117, -124,
   -127, -124, -117, -105,  -89,  -70,  -48,  -24
};

static const int8_t grad_table[32] PROGMEM = {
     24,  24,  22,  19,  16,  12,   7,   3,
     -3,  -7, -12, -16, -19, -22, -24, -24,
    -24, -24, -22, -19, -16, -12,  -7,  -3,
      3,   7,  12,  16,  19,  22,  24,  24
};

// Table of 12-bit Gamma correct values converting 0..255 linear space to
// 12-bit gamma space where gamma=1.5
//
// Calculated in Python using:
//
//     [ int(((i/255.0) ** 1.5)* 4096) for i in range(0,256) ]
//
// const uint16_t gamma16_table[256] = {
//     0, 1, 2, 5, 8, 11, 14, 18,
//     22, 27, 31, 36, 41, 47, 52, 58,
//     64, 70, 76, 83, 89, 96, 103, 110,
//     118, 125, 133, 141, 149, 157, 165,173,
//     182, 190, 199, 208, 217, 226, 235, 244,
//     254, 264, 273, 283, 293, 303, 313, 324,
//     334, 345, 355, 366, 377, 388, 399, 410,
//     421, 432, 444, 455, 467, 479, 491, 502,
//     515, 527, 539, 551, 564, 576, 589, 601,
//     614, 627, 640, 653, 666, 679, 692, 706,
//     719, 733, 746, 760, 774, 788, 802, 816,
//     830, 844, 858, 873, 887, 902, 916, 931,
//     946, 960, 975, 990, 1005, 1021, 1036, 1051,
//     1066, 1082, 1097, 1113, 1128, 1144, 1160, 1176,
//     1192, 1208, 1224, 1240, 1256, 1273, 1289, 1305,
//     1322, 1338, 1355, 1372, 1388, 1405, 1422, 1439,
//     1456, 1473, 1490, 1508, 1525, 1542, 1560, 1577,
//     1595, 1612, 1630, 1648, 1666, 1684, 1702, 1720,
//     1738, 1756, 1774, 1792, 1811, 1829, 1847, 1866,
//     1885, 1903, 1922, 1941, 1959, 1978, 1997, 2016,
//     2035, 2054, 2074, 2093, 2112, 2131, 2151, 2170,
//     2190, 2209, 2229, 2249, 2269, 2288, 2308, 2328,
//     2348, 2368, 2388, 2408, 2429, 2449, 2469, 2490,
//     2510, 2531, 2551, 2572, 2592, 2613, 2634, 2655,
//     2676, 2697, 2718, 2739, 2760, 2781, 2802, 2823,
//     2845, 2866, 2887, 2909, 2930, 2952, 2974, 2995,
//     3017, 3039, 3061, 3082, 3104, 3126, 3148, 3171,
//     3193, 3215, 3237, 3259, 3282, 3304, 3327, 3349,
//     3372, 3394, 3417, 3440, 3462, 3485, 3508, 3531,
//     3554, 3577, 3600, 3623, 3646, 3670, 3693, 3716,
//     3739, 3763, 3786, 3810, 3833, 3857, 3881, 3904,
//     3928, 3952, 3976, 4000, 4023, 4047, 4071, 4096
// };

const uint8_t gamma8_table[256] = {
    0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 2, 2, 2, 3, 3,
    4, 4, 4, 5, 5, 6, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10,
    11, 11, 12, 13, 13, 14, 14, 15,
    15, 16, 17, 17, 18, 18, 19, 20,
    20, 21, 22, 22, 23, 24, 24, 25,
    26, 27, 27, 28, 29, 29, 30, 31,
    32, 32, 33, 34, 35, 36, 36, 37,
    38, 39, 40, 40, 41, 42, 43, 44,
    44, 45, 46, 47, 48, 49, 50, 51,
    51, 52, 53, 54, 55, 56, 57, 58,
    59, 60, 60, 61, 62, 63, 64, 65,
    66, 67, 68, 69, 70, 71, 72, 73,
    74, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89,
    91, 92, 93, 94, 95, 96, 97, 98,
    99, 100, 101, 103, 104, 105, 106, 107,
    108, 109, 110, 112, 113, 114, 115, 116,
    117, 118, 120, 121, 122, 123, 124, 126,
    127, 128, 129, 130, 132, 133, 134, 135,
    136, 138, 139, 140, 141, 143, 144, 145,
    146, 148, 149, 150, 151, 153, 154, 155,
    156, 158, 159, 160, 162, 163, 164, 165,
    167, 168, 169, 171, 172, 173, 175, 176,
    177, 179, 180, 181, 183, 184, 185, 187,
    188, 189, 191, 192, 194, 195, 196, 198,
    199, 200, 202, 203, 205, 206, 207, 209,
    210, 212, 213, 215, 216, 217, 219, 220,
    222, 223, 225, 226, 227, 229, 230, 232,
    233, 235, 236, 238, 239, 241, 242, 244,
    245, 247, 248, 250, 251, 252, 254, 255
 };


// Valid seeds for the random number generators.
//
static uint16_t g_seed16 = 36243;
static uint32_t g_seed32 = 2463534242;


// Mathematical functions -----------------------------------------------------

// Generate pseudorandom numbers, based on "Xorshift RNGs", George
// Marsaglia, 2003
//
//    http://www.jstatsoft.org/v08/i14/paper
//

// Set the seed for the 16-bit pseudorandom sequence.
void set_seed16(uint16_t seed) { g_seed16 = seed; }

// Return a 16-bit pseudorandom value.
uint16_t random16(void)
{
    g_seed16 ^= (g_seed16 << 13);
    g_seed16 ^= (g_seed16 >> 9);
    g_seed16 ^= (g_seed16 << 7);
    return g_seed16;
}


// Set the seed for the 32-bit pseudorandom sequence.
void set_seed32(uint32_t seed) { g_seed32 = seed; }

// Return a 32-bit pseudorandom value.
//
uint32_t random32(void)
{
    g_seed32 ^= (g_seed32 << 13);
    g_seed32 ^= (g_seed32 >> 17);
    g_seed32 ^= (g_seed32 << 5);
    return g_seed32;
}

// Sine and cosine functions.
// Angles are in the 0..255 radians format.
//
int8_t sintable(uint8_t angle)
{
   uint8_t index = angle >> 3;
   int8_t result = pgm_read_byte(&sin_table[index]);
   // add in the signed gradient
   int8_t interp = angle & 0x7;
   int8_t grad = pgm_read_byte(&grad_table[index]);
   result += (interp * grad) >> 3;
   return result;
}

// Linear interpolation between two values.
// 8-bit fixed point values where 0 = 0.0f and 255 = 1.0f
//
uint8_t lerp(uint8_t high, uint8_t low, uint8_t t)
{
    int16_t temp = t * (high-low);
    return low + (temp >> 8);
}

// restrict a signed value to within a range.
//
int8_t clamp(int8_t value, int8_t low, int8_t high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

// restrict a float to within a range.
//
float clampf(float value, float low, float high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}


uint8_t smoothstep(uint8_t min, uint8_t max, uint8_t t)
{
    if (t < min) return 0;
    if (t > max) return 255;
    uint8_t x = (t-min) / (max - min);
    return 3 * x * x - 2 * x * x * x;
}

// return a 32-bit value containing a random color on the spectrum
// (i.e. a full saturation hue with no intensity).
uint32_t random_color(void)
{
    uint8_t phase = random16() >> 8;
    uint8_t r = sintable(phase) + 127;
    uint8_t g = sintable(phase + 85) + 127;   // plus 2Pi/3
    uint8_t b = sintable(phase + 170) + 127;  // plus 2*2Pi/3
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint16_t rightmost_bit_16(uint16_t value)
{
    // return the rightmost set bit in a 16-bit value (the bit, not the bit
    // position), or zero of no bits are set.
    uint16_t bit = 0x0001;
    while (bit && (value & bit) == 0) {
        bit <<= 1;
    }
    return bit;
}

// ----------------------------------------------------------------------------

uint16_t rotate16_right(const uint16_t value)
{
    // Rotate a 16-bit value (formatted as a 4x4 grid) 90 degrees right, e.g.
    //
    //  1  2  3  4      13 9  5  1
    //  5  6  7  8  ->  14 10 6  2
    //  9  10 11 12     15 11 7  3
    //  13 14 15 16     16 12 8  4
    //
    uint8_t row[4];
    row[0] = (value & 0xf000) >> 12;
    row[1] = (value & 0x0f00) >> 8;
    row[2] = (value & 0x00f0) >> 4;
    row[3] = (value & 0x000f);
    uint8_t col[4] = {0,0,0,0};
    uint8_t bit = 0x01;
    for (uint8_t i=0; i<4; i++) {
        col[0] |= (row[i] & 0x08) ? bit : 0;
        col[1] |= (row[i] & 0x04) ? bit : 0;
        col[2] |= (row[i] & 0x02) ? bit : 0;
        col[3] |= (row[i] & 0x01) ? bit : 0;
        bit <<= 1;
    }
    return (col[0] << 12) | (col[1] << 8) | (col[2] << 4) | col[3];
}

uint16_t rotate16_left(const uint16_t value)
{
    // Rotate a 16-bit value (formatted as a 4x4 grid) 90 degrees left, e.g.
    //
    //  1  2  3  4      4  8  12 16
    //  5  6  7  8  ->  3  7  11 15
    //  9  10 11 12     2  6  10 14
    //  13 14 15 16     1  5  9  13
    //
    uint8_t row[4];
    row[0] = (value & 0xF000) >> 12;
    row[1] = (value & 0x0F00) >> 8;
    row[2] = (value & 0x00F0) >> 4;
    row[3] = (value & 0x000F);
    uint8_t col[4] = {0,0,0,0};
    uint8_t bit = 0x08;
    for (uint8_t i=0; i<4; i++) {
        col[0] |= (row[i] & 0x01) ? bit : 0;
        col[1] |= (row[i] & 0x02) ? bit : 0;
        col[2] |= (row[i] & 0x04) ? bit : 0;
        col[3] |= (row[i] & 0x08) ? bit : 0;
        bit >>= 1;
    }
    return (col[0] << 12) |(col[1] << 8) | (col[2] << 4) | col[3];
}

// ----------------------------------------------------------------------------
