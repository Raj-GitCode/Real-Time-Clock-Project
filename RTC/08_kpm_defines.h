//kpm_defines.h
#ifndef KPM_DEFINES_H
#define KPM_DEFINES_H

// Keypad row pins (outputs)
#define ROW0 16  // P1.19
#define ROW1 17  // P1.20
#define ROW2 18  // P1.21
#define ROW3 19  // P1.22

// Keypad column pins (inputs)
#define COL0 20  // P1.16
#define COL1 21  // P1.17
#define COL2 22  // P1.18
#define COL3 23  // P1.23

// All rows mask
#define ROWS_MASK ((1<<ROW0)|(1<<ROW1)|(1<<ROW2)|(1<<ROW3))
// All columns mask
#define COLS_MASK ((1<<COL0)|(1<<COL1)|(1<<COL2)|(COL3))

#endif
