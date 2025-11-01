
#include <LPC214x.h>
#include "types.h"
#include "lcd.h"
#include "rtc.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "adc.h"
#include "dt.h"

// Global variables
s32 hour, min, sec, date, month, year, day;
volatile unsigned char eint_flag = 0;
volatile unsigned char menu_active = 0;
char key;

// Alarm globals
s32 alarm_hour = -1, alarm_min = -1, alarm_sec = -1;

// Function prototypes (defined in main_helpers.c)
void EINT0_Init(void);
void Check_Alarm(void);
void Show_RTC_Display(void);
void Show_ADC_Temperature(void);
void Show_Edit_Menu(void);

// External interrupt ISR
void EINT0_ISR(void) __irq
{
    eint_flag = 1;           // Set flag to show menu
    EXTINT = 0x01;           // Clear EINT0 flag
    VICVectAddr = 0x00;      // Acknowledge end of interrupt
}

#include "bell.h"

int main(void)
{
    Init_LCD();
    RTC_Init();
    Init_KPM();
    Init_ADC();
		//Create_Bell_Symbol();   // Create alarm bell character

    IODIR0 |= (1 << 0);  // Set P0.0 as output (LED)
    IOCLR0 = (1 << 0);   // Turn LED OFF initially

    Show_ADC_Temperature();

    // Set initial date/time
    SetRTCTimeInfo(19, 30, 0);
    SetRTCDateInfo(18, 10, 2025);
    SetRTCDay(6); // Saturday

    // Initialize EINT0
    EINT0_Init();
    CmdLCD(0x0C);   // Display ON, cursor OFF, blink OFF

    while (1)
    {
        if (eint_flag)
        {
            menu_active = 1;
            eint_flag = 0;
            Show_Edit_Menu();
        }

        if (!menu_active)
        {
            Show_RTC_Display();     // Show RTC only if menu not active
            Show_ADC_Temperature();
            Check_Alarm();          // LED control happens here
        }
    }
}


