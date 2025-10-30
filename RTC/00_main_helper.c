//main_helper.c
#include <LPC214x.h>
#include "types.h"
#include "lcd.h"
#include "rtc.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "adc.h"
#include "dt.h"


void Set_DateAlarm(void);
void Set_Alarm(void);
// Extern globals from main.c
extern s32 hour, min, sec, date, month, year, day;
extern s32 alarm_hour, alarm_min, alarm_sec;
extern volatile unsigned char menu_active;
extern char key;

// External ISR from main.c
extern void EINT0_ISR(void);

void Check_Alarm(void)
{
    u32 i;
    static unsigned char alarm_triggered = 0;

    if (alarm_hour == -1) return;  // No alarm set

    if (hour == alarm_hour && min == alarm_min && sec == alarm_sec && alarm_triggered == 0)
    {
        alarm_triggered = 1;
        IOSET0 = (1 << 0);    // Turn ON LED
        for (i = 0; i < 5; i++)
            delay_s(1);
        IOCLR0 = (1 << 0);    // Turn OFF LED
    }

    if (sec != alarm_sec)
        alarm_triggered = 0;
}

void EINT0_Init(void)
{
    // Configure P0.1 as EINT0
    PINSEL0 &= ~(3 << 2);
    PINSEL0 |=  (3 << 2);   // P0.1 = EINT0

    EXTMODE  = 0x01;        // Edge sensitive
    EXTPOLAR = 0x00;        // Falling edge
    EXTINT   = 0x01;        // Clear flag

    // VIC setup
    VICIntSelect = 0x00000000;
    VICVectAddr0 = (unsigned long)EINT0_ISR;
    VICVectCntl0 = 0x20 | 14;
    VICIntEnable = (1 << 14);
}

void Show_RTC_Display(void)
{
    static int last_min = -1;   // Remember the last displayed minute

    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&day);

    // Clear LCD only when minute changes (once per minute)
    if (min != last_min)
    {
        CmdLCD(CLEAR_LCD);
        last_min = min;
    }

    DisplayRTCTime(hour, min, sec);
    DisplayRTCDay(day);
    DisplayRTCDate(date, month, year);
    CmdLCD(0x0C);
    delay_ms(2);
}

void Show_ADC_Temperature(void)
{
    unsigned int adcValue;
    float voltage, temperature;
    int intPart, decPart;

    adcValue = Read_ADC(1);
    voltage = (adcValue * 3.3f) / 1023.0f;
    temperature = voltage * 100.0f;

    intPart = (int)temperature;
    decPart = (int)((temperature - intPart) * 10 + 0.5f);

    CmdLCD(GOTO_LINE1_POS0 + 11);
    U32LCD(intPart);
    CharLCD('.');
    U32LCD(decPart);
    CharLCD(223);
    StrLCD("C");
}

void Show_Edit_Menu(void)
{
    CmdLCD(CLEAR_LCD);
    StrLCD("  ****EDIT****");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("1.SDT 2.SA 3.EXT");

    while (1)
    {
        key = KeyScan();

        switch (key)
        {
            case '1':
                Set_DateAlarm();
                CmdLCD(CLEAR_LCD);
                StrLCD("  ****EDIT****");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("1.SDT 2.SA 3.EXT");
                break;

            case '2':
                Set_Alarm();
                CmdLCD(CLEAR_LCD);
                StrLCD("  ****EDIT****");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("1.SDT 2.SA 3.EXT");
                break;

            case '3':
                CmdLCD(CLEAR_LCD);
                menu_active = 0;
                return;

            default:
                break;
        }
    }
}
