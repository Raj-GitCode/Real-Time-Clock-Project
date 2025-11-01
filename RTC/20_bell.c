//bell.c
#include "lcd.h"
#include "delay.h"
#include "types.h"
#include "lcd_defines.h"

void Show_Bell_On_LCD(void)
{
    static u8 bell_symbol[8] = {
        0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04, 0x00
    };

    BuildCGRAM(bell_symbol, 8);    // load bell icon into CGRAM

    CmdLCD(CLEAR_LCD);

    // first line center position for bell (pos ~7 on 16x2 LCD)
    CmdLCD(GOTO_LINE1_POS0 + 7);
    CharLCD(0);    // bell symbol (from CGRAM)

    // second line message centered
    //CmdLCD(GOTO_LINE2_POS0 + 3);
    //StrLCD("** ALARM **");
		CmdLCD(GOTO_LINE2_POS0);
    StrLCD("Press * 2ExtAlrm*");
}
