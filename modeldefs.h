// Midifighter Pro model definitions
//
//   Copyright (C) 2011 Robin Green
//
// rgreen 2011-03-12

#ifndef _MODELDEFS_H_INCLUDED
#define _MODELDEFS_H_INCLUDED

// CUEMASTER
// ---------
//
//                  USB
//                   |
//  +----------------#---------------+
//  |                   sw0          |
//  |  (R7)  (R8)  [s1]  O  O  O  O  |
//  |                        ^       |
//  |   |     |    [s2]  O  O| O  O  |
//  |   =     =              |       |
//  |   |     |    [s3]  O  O| O  O  |
//  |   R5    R6             |   sw16|
//  |   |     |    [s4]  O  O  O  O  |
//  |                                |
//  +--------------------------------+

#ifdef TRAKTOR_H
#warning Setting up for MidifighterPro Traktor_H
// no keypad rotation
// no ext key reordering
// invert sliders
//#define KEYGRID_ROTATE_LEFT // Include for XX Fader
#define INVERT_SLIDER_3   // Remove for XX Fader
#define INVERT_SLIDER_4   // Remove for XX Fader
#endif

// BEATMASHER
// -----
//
//     +-----------------+
//     |  R              |
//     | (12) [7][5]  |  |
//     |      [8][6]  =  |
//     |              |  |
//     |             R10 |
//     |  -|---R11-   |  |
//     |                 |
//     |  sw0            |   U
//     |   O  O  O  O    #== S
//     |                 |   B
//     |   O  O  O  O    |
//     |     ------->    |
//     |   O  O  O  O    |
//     |           sw16  |
//     |   O  O  O  O    |
//     |                 |
//     +-----------------+

#ifdef TRAKTOR_V
#warning Setting up for MidifighterPro Traktor_V
// keypad rotation
#define KEYGRID_ROTATE_RIGHT
#define INVERT_SLIDER_2
// no ext key reordering
// no invert sliders
#endif


// SUPER_KNOB
// ------
//
//     +-----------------+
//     | R   sw   sw  R  |
//     |(12)[11] [09](16)|
//     |                 |
//     |                 |
//     | R   sw   sw  R  |
//     |(14)[12] [10](15)|
//     |                 |
//     |  sw0            |   U
//     |   O  O  O  O    #== S
//     |                 |   B
//     |   O  O  O  O    |
//     |     ------->    |
//     |   O  O  O  O    |
//     |           sw16  |
//     |   O  O  O  O    |
//     |                 |
//     +-----------------+

#ifdef SERATO
#warning Setting up for MidifighterPro Serato
// keypad rotation
#define KEYGRID_ROTATE_RIGHT
// ext button reordering
#define REORDER_EXT_KEYS
// no invert sliders
#endif

#endif // _MODELDEFS_H_INCLUDED
