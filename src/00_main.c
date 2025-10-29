
//main.c
#include <LPC214x.h>
#include "types.h"
#include "lcd.h"
#include "rtc.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"     // ? For key_scan()
#include "dt.h"
#include "adc_defines.h"
#include "adc.h"

// Global variables
#define BUZZER_PIN 23
s32 hour, min, sec, date, month, year, day;
u8 eint_flag = 0;
u8 menu_active = 0;
u8 key;

// Function prototypes
void EINT0_Init(void);
void Show_Edit_Menu(void);
void Show_RTC_Display(void);
void Show_ADC_Temperature(void);

void EINT0_ISR(void) __irq
{
    eint_flag = 1;           // Set flag to show menu
    EXTINT = 0x01;           // Clear EINT0 flag
    VICVectAddr = 0x00;      // Acknowledge end of interrupt
}

int main(void)
{
    // Initialize LCD, RTC, and keypad
    Init_LCD();
    RTC_Init();
    Init_KPM();   // ? Initialize keypad (if required in your setup)
		Init_ADC();     // <<< IMPORTANT: Initialize ADC here!
	
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
            Show_RTC_Display();   // Show RTC only if menu not active
						Show_ADC_Temperature(); 
        }
    }
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
    VICIntSelect = 0x00000000;                // IRQ
    VICVectAddr0 = (unsigned long)EINT0_ISR; // ISR
    VICVectCntl0 = 0x20 | 14;                 // Slot 0
    VICIntEnable = (1 << 14);                 // Enable EINT0
}
#include "adc.h"  // Make sure ADC functions are included
void Show_ADC_Temperature(void)
{
    unsigned int adcValue;
    float voltage, temperature;
    int intPart, decPart;

    // Read ADC and calculate temperature
    adcValue = Read_ADC(0);                     // ADC channel 0
    voltage = (adcValue * 3.3f) / 1023.0f;     // Convert ADC to voltage
    temperature = voltage * 100.0f;            // LM35: 10mV per °C

    intPart = (int)temperature;                                 // Integer part
    decPart = (int)((temperature - intPart) * 10 + 0.5f);       // ONE decimal digit

    // Move cursor to line 2, column 12
    CmdLCD(GOTO_LINE2_POS0 + 11);

    // Display temperature
    U32LCD(intPart);                      // Integer part
    CharLCD(223);                         // Degree symbol (°)
    CharLCD('.');                         // Decimal point
    U32LCD(decPart);                      // One decimal digit
    StrLCD("C");                          // °C
}



void Show_RTC_Display(void)
{
    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&day);

    //CmdLCD(CLEAR_LCD);
    DisplayRTCTime(hour, min, sec);
    DisplayRTCDay(day);
    DisplayRTCDate(date, month, year);
	
		
		CmdLCD(0x0C);  // Display ON, cursor OFF, blink OFF

    //delay_ms(1);
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
								Set_DateAlarm(); // updates rtc_hour, rtc_min, etc.
								
								// Update actual RTC
								SetRTCTimeInfo(rtc_hour, rtc_min, rtc_sec);
								SetRTCDateInfo(rtc_date, rtc_month, rtc_year);
								SetRTCDay(rtc_day);

								// Refresh menu display
								CmdLCD(CLEAR_LCD);
								StrLCD("  ****EDIT****");
								CmdLCD(GOTO_LINE2_POS0);
								StrLCD("1.SDT 2.SA 3.EXT");
								break;

            case '2':
                Set_Alarm();  // <<< Call the alarm time editor
                //DisplayRTCTime(hour, min, sec); // Optional: redraw main display after returning
								CmdLCD(CLEAR_LCD);
								StrLCD("  ****EDIT****");
								CmdLCD(GOTO_LINE2_POS0);
								StrLCD("1.SDT 2.SA 3.EXT");
                break;
            case '3':
                CmdLCD(CLEAR_LCD);
                menu_active = 0;
                return;   // Exit the menu function

            default:
                break;  // Ignore other keys
        }
    }
}

