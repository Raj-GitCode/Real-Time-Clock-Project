//DateTime.c
#include "dt.h"
// ---- Function Declarations ----
void Display_Template(void);
void Handle_D_Field(u8 key);
void MoveCursorVertically(u8 key);
void MoveCursor(u8 key);
void MoveCursorBack(void);
void EnterDigit(u8 key);


// RTC Date/Time
u8 rtc_hour   = 0;
u8 rtc_min    = 0;
u8 rtc_sec    = 0;
u8 rtc_date   = 0;
u8 rtc_month  = 0;
u16 rtc_year  = 0;
u8 rtc_day    = 0; // 0..6
// ---- LCD Setup ----
void Display_Template(void)
{
    CmdLCD(0x01); // Clear display
    StrLCD((s8 *)"DATE: DD:MM:YYYY");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((s8 *)"TIME: HH:MM:SS D");
    CmdLCD(0x06);
    CmdLCD(0x0E);
    cursor_pos = 0;
}

// ---- Add Field ----
void AddField(u8 start, u8 len, u16 max)
{
    Field *f;
    u8 i;

    f = &fields[total_fields++];
    f->start_pos = start;
    f->length = len;
    f->max_value = max;

    for (i = 0; i < len; i++) {
        f->digits[i] = '0';
        f->filled[i] = 0;
    }
}

// ---- Cursor Position ----
void SetCursorPos(u8 pos)
{
    if (pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (pos - 16));

    cursor_pos = pos;
}

// ---- Get Field Based on Cursor ----
Field* GetCurrentField(void)
{
    u8 i;
    for (i = 0; i < total_fields; i++) {
        if (cursor_pos >= fields[i].start_pos &&
            cursor_pos < (fields[i].start_pos + fields[i].length))
            return &fields[i];
    }
    return NULL;
}

// ---- Move Cursor Back ----
void MoveCursorBack(void)
{
    if (cursor_pos == 6)
        return;

    if (cursor_pos == (16 + 6))
        cursor_pos = 15;
    else {
        cursor_pos--;
        if (cursor_pos == 8 || cursor_pos == 11 ||
            cursor_pos == 24 || cursor_pos == 27 || cursor_pos == 30)
            cursor_pos--;
    }
}

// ---- Move Cursor ----
void MoveCursor(u8 key)
{
    Field *f;

    if (key == '+') {
        if (cursor_pos >= 31)
            cursor_pos = 31;
        else if (cursor_pos == 15) {
            f = GetCurrentField();
            if (f != NULL)
                AutoFillField(f);
            CmdLCD(GOTO_LINE2_POS0 + 6);
            cursor_pos = 22;
        } else {
            f = GetCurrentField();
            if (f != NULL) {
                u8 last_pos = f->start_pos + f->length - 1;
                if (cursor_pos == last_pos)
                    AutoFillField(f);
            }
            cursor_pos++;
        }
    } else if (key == '=')
        MoveCursorBack();

    if (cursor_pos > 31)
        cursor_pos = 31;

    if (cursor_pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + cursor_pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (cursor_pos - 16));
}

// ---- Auto-fill Field ----
void AutoFillField(Field *f)
{
    u8 i;
    for (i = 0; i < f->length; i++) {
        if (!f->filled[i]) {
            f->digits[i] = '0';
            SetCursorPos(f->start_pos + i);
            CharLCD('0');
            f->filled[i] = 1;
        }
    }
    CheckAndFixField(f);
    SetCursorPos(f->start_pos + f->length);
}

// ---- Validate Field ----
void CheckAndFixField(Field *f)
{
    u8 i;
    u16 value = 0;

    for (i = 0; i < f->length; i++)
        if (!f->filled[i]) return;

    for (i = 0; i < f->length; i++)
        value = (value * 10)+ (f->digits[i] - '0');

    if (value > f->max_value) {
        for (i = 0; i < f->length; i++) {
            f->digits[i] = '0';
            SetCursorPos(f->start_pos + i);
            CharLCD('0');
        }
        SetCursorPos(cursor_pos);
    }
}

// ---- Check if Cursor in Editable Area ----
u8 IsEditablePos(u8 pos)
{
    u8 i;
    for (i = 0; i < total_fields; i++) {
        if (pos >= fields[i].start_pos &&
            pos < (fields[i].start_pos + fields[i].length))
            return 1;
    }
    return 0;
}

// ---- Enter Digit ----
void EnterDigit(u8 key)
{
    Field *f;
    u8 idx;

    if (!IsEditablePos(cursor_pos))
        return;

    if (cursor_pos == 31) {
        Handle_D_Field(key);
        return;
    }

    if (key >= '0' && key <= '9') {
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
}

// ---- Handle D Field ----
void Handle_D_Field(u8 key)
{
    static u8 D_value = 0;

    if (key >= '0' && key <= '9') {
        D_value = key - '0';
        if (D_value > 6)
            D_value = 0;
        SetCursorPos(31);
        CharLCD(D_value + '0');
        CmdLCD(0x10);
    }
}

// ---- Move Cursor Vertically ----
void MoveCursorVertically(u8 key)
{
    if (key == '-') {
        if (cursor_pos < 16)
            cursor_pos += 16;
    } else if (key == 'x') {
        if (cursor_pos >= 16)
            cursor_pos -= 16;
    }

    if (cursor_pos < 16)
        CmdLCD(GOTO_LINE1_POS0 + cursor_pos);
    else
        CmdLCD(GOTO_LINE2_POS0 + (cursor_pos - 16));
}

// ---- MAIN ----
void  Set_DateAlarm(void)
{
    u8 key;

    Init_LCD();
    Init_KPM();

    Display_Template();

    // ---- Add Fields ----
    AddField(6, 2, 31);     // DD
    AddField(9, 2, 12);     // MM
    AddField(12, 4, 1111);  // YYYY
    AddField(22, 2, 23);    // HH
    AddField(25, 2, 59);    // MM
    AddField(28, 2, 59);    // SS
    AddField(31, 1, 6);     // D field

    SetCursorPos(6);

    while (1)
    {
        key = KeyScan();

        if (key != '\0') {
            if (key == '+' || key == '=')
                MoveCursor(key);
            else if (key == '-' || key == 'x')
                MoveCursorVertically(key);
            else if (key >= '0' && key <= '9')
                EnterDigit(key);
					  //else if (key == 'b' || key == 'B')
						//		return;
						if (key == 'b' || key == 'B') {
								rtc_date  = (fields[0].digits[0]-'0')*10 + (fields[0].digits[1]-'0');
								rtc_month = (fields[1].digits[0]-'0')*10 + (fields[1].digits[1]-'0');
								rtc_year  = (fields[2].digits[0]-'0')*1000 + (fields[2].digits[1]-'0')*100
														+ (fields[2].digits[2]-'0')*10 + (fields[2].digits[3]-'0');
								rtc_hour  = (fields[3].digits[0]-'0')*10 + (fields[3].digits[1]-'0');
								rtc_min   = (fields[4].digits[0]-'0')*10 + (fields[4].digits[1]-'0');
								rtc_sec   = (fields[5].digits[0]-'0')*10 + (fields[5].digits[1]-'0');
								rtc_day   = fields[6].digits[0] - '0';
							
								    // Show confirmation message
								CmdLCD(CLEAR_LCD);              // Clear LCD
								StrLCD("DATE TIME SAVED");      // Show message
								delay_ms(500);                  // Wait 500 ms (0.5 seconds) for user to see message

								return;
						}
            delay_ms(200);
        }
    }
}
