//dt.h
#ifndef DT_H
#define DT_H

#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "kpm.h"
#include "delay.h"
#include <stddef.h>   // for NULL

// Maximum number of editable fields (e.g., HH, MM, SS, Day, Month, Year, etc.)
#define MAX_FIELDS 7  

typedef struct {
    u8 start_pos;       // absolute LCD position (0–31)
    u8 length;          // number of digits in the field
    u16 max_value;      // maximum allowed value
    u8 digits[4];       // digit buffer (supports up to 4 digits)
    u8 filled[4];       // filled flags for each digit
} Field;

// Global variables
extern u8 cursor_pos;           // current cursor position on LCD
extern u8 total_fields;         // total number of fields
extern Field fields[MAX_FIELDS]; // array of fields

// Function declarations
void AddField(u8 start, u8 len, u16 max);   // Add a field definition
void SetCursorPos(u8 pos);                  // Set cursor position
Field* GetCurrentField(void);               // Get pointer to current field
void AutoFillField(Field *f);               // Auto-fill unfilled digits
void CheckAndFixField(Field *f);            // Validate & fix field value
u8 IsEditablePos(u8 pos);                   // Check if cursor pos is editable


void  Set_DateAlarm(void);
void Set_Alarm(void);

// RTC Date/Time
extern u8 rtc_hour;
extern u8 rtc_min;
extern u8 rtc_sec;
extern u8 rtc_date;
extern u8 rtc_month;
extern u16 rtc_year;
extern u8 rtc_day;

// Alarm Date/Time
extern u8 alarm_hour;
extern u8 alarm_min;
extern u8 alarm_sec;
extern u8 alarm_date;


#endif // DT_H

