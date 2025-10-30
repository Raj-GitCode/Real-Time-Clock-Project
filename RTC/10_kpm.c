//kpm.c
#include <LPC21xx.h>
#include "types.h"
#include "kpm_defines.h"
#include "lcd.h"
#include "lcd_defines.h"

// Keypad lookup table
/*
u8 kpmLUT[4][3] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
*/
// 4x4 Keypad lookup table
u8 kpmLUT[4][4] = {
    {'7','8','9','/'},
    {'4','5','6','x'},
    {'1','2','3','-'},
    {'b','0','=','+'}
};

// Initialize Keypad: rows output, columns input
void Init_KPM(void)
{
    // Rows as output
    IODIR1 |= ROWS_MASK;
    // Ground all rows
    IOCLR1 = ROWS_MASK;

    // Columns as input (default)
    IODIR1 &= ~COLS_MASK;
}

// Scan keypad and return ASCII of pressed key
char KeyScan(void)
{
    unsigned int rows[4] = {ROW0, ROW1, ROW2, ROW3};
    unsigned int cols[4] = {COL0, COL1, COL2,COL3};
    unsigned int i,j;

    for(i=0; i<4; i++)
    {
        // Set all rows HIGH
        IOSET1 = ROWS_MASK;
        // Pull current row LOW
        IOCLR1 = (1<<rows[i]);
        
        // Short delay for debounce
        for(j=0;j<1000;j++);

        // Scan columns
        for(j=0; j<4; j++)
        {
            if((IOPIN1 & (1<<cols[j])) == 0)
            {
                // Wait until key released
                while((IOPIN1 & (1<<cols[j])) == 0);
                return kpmLUT[i][j];
            }
        }
    }
    return '\0'; // No key pressed
}

// Read multi-digit numeric input
void ReadNum(u32 *num, u8 *key)
{
    *num = 0;
    while(1)
    {
        *key = KeyScan();
        if(*key >= '0' && *key <= '9')
        {
            *num = (*num * 10) + (*key - '0');
            CmdLCD(GOTO_LINE2_POS0);
            U32LCD(*num);
        }
        else if(*key != '\0')
        {
            CharLCD(' ');
            CharLCD(*key);
            break;
        }
    }
}
