//Time.c
#include "dt.h"

// Alarm Date/Time
u8 alarm_hour   = 0;
u8 alarm_min    = 0;
u8 alarm_sec    = 0;

// Definitions (actual memory allocation)
u8 cursor_pos = 0;
u8 total_fields = 0;
Field fields[MAX_FIELDS];

// ---- Function Declarations ----

void Display_AlarmTemplate(void);
void MoveCurs(u8 key);
void MoveCursBack(void);
void EnterDig(u8 key);

// ---- LCD Setup ----
void Display_AlarmTemplate(void)
{
    CmdLCD(0x01);
    StrLCD((s8 *)"***Alarm Time***");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((s8 *)"   HH:MM:SS   ");
    CmdLCD(0x06);
    CmdLCD(0x0E);
    cursor_pos = 3;      // start at first H
}

// ---- Move Cursor Back ----
void MoveCursBack(void)
{
    if (cursor_pos == 19) return;  // first editable pos
    cursor_pos--;
    if (cursor_pos == 21 || cursor_pos == 24) cursor_pos--; // skip colons
}

// ---- Move Cursor ----
void MoveCurs(u8 key)
{
    Field *f;

    if (key == '+') {
        f = GetCurrentField();
        if (f != NULL) {
            u8 last = f->start_pos + f->length - 1;
            if (cursor_pos == last) AutoFillField(f);
        }

        cursor_pos++;
        if (cursor_pos == 21 || cursor_pos == 24) cursor_pos++; // skip :
        if (cursor_pos > 26) cursor_pos = 26;
    }
    else if (key == '=')
        MoveCursBack();

    if (cursor_pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + cursor_pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (cursor_pos - 16));
}
// ---- Enter Digit ----
void EnterDig(u8 key)
{
    Field *f;
    u8 idx;

    if (!IsEditablePos(cursor_pos)) return;
    if (key < '0' || key > '9') return;

    f = GetCurrentField();
    if (f != NULL) {
        CharLCD(key);
        CmdLCD(0x10);

        idx = cursor_pos - f->start_pos;
        if (idx < f->length) {
            f->digits[idx] = key;
            f->filled[idx] = 1;
            CheckAndFixField(f);
        }
    }
}

// ---- MAIN ----
void Set_Alarm(void)
{
    u8 key;

    Init_LCD();
    Init_KPM();

    Display_AlarmTemplate();

    // ---- Add Fields ----
    AddField(19, 2, 23);   // HH
    AddField(22, 2, 59);   // MM
    AddField(25, 2, 59);   // SS

    SetCursorPos(19);      // start at first H

    while (1) {
        key = KeyScan();

        if (key != '\0') 
				{
            if (key == '+' || key == '=')
                MoveCurs(key);
            else if (key >= '0' && key <= '9')
                EnterDig(key);
					//  if (key == 'b' || key == 'B')
					//		return;
						if (key == 'b' || key == 'B') {
								alarm_hour = (fields[0].digits[0]-'0')*10 + (fields[0].digits[1]-'0');
								alarm_min  = (fields[1].digits[0]-'0')*10 + (fields[1].digits[1]-'0');
								alarm_sec  = (fields[2].digits[0]-'0')*10 + (fields[2].digits[1]-'0');
								
							CmdLCD(CLEAR_LCD);              // Clear LCD
							StrLCD("ALARM SAVED");      // Show message
							delay_ms(500);  
								return;
						}
						                // Wait 500 ms (0.5 seconds) for user to see message
        }
    }
}
