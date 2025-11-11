# Real-Time-Clock-Project



# 🕒 LPC2148 Real-Time Clock (RTC) with LCD, Keypad, ADC (LM35), and Alarm System

A complete **embedded system project** for the **LPC2148 (ARM7TDMI-S)** microcontroller.  
This project integrates an **RTC module**, **16x2 LCD display**, **4x4 keypad**, **ADC-based temperature sensor (LM35)**, and **alarm control with interrupt and buzzer**.

---

## 🚀 Features

✅ Real-Time Clock (RTC) display (Date, Time, Day)  
✅ Set Date/Time and Alarm using 4x4 matrix keypad  
✅ ADC temperature reading using LM35 sensor (displayed in °C)  
✅ Alarm trigger with buzzer and bell icon on LCD  
✅ External interrupt (EINT0) opens edit menu  
✅ Automatic and manual alarm stop  
✅ Custom LCD characters (Bell symbol, degree symbol °)  
✅ Modular code structure for easy maintenance and reusability  

---

## 🧠 Project Architecture

📂 LPC2148_RTC_Alarm_System
├── main.c
├── main_helper.c
├── lcd.c / lcd.h / lcd_defines.h
├── kpm.c / kpm.h / kpm_defines.h
├── rtc.c / rtc.h / rtc_defines.h
├── adc.c / adc.h / adc_defines.h
├── delay.c / delay.h
├── setDateTime.c
├── setAlarm.c
├── bell.c / bell.h
├── types.h / defines.h
└── README.md

yaml
Copy code

---

## 🧰 Hardware Requirements

| Component | Description |
|------------|-------------|
| **Microcontroller** | LPC2148 (ARM7TDMI-S) |
| **Display** | 16x2 LCD (HD44780 compatible) |
| **Keypad** | 4x4 Matrix keypad |
| **Temperature Sensor** | LM35 connected to ADC0.1 (P0.28) |
| **Buzzer/LED** | Connected to P0.0 |
| **External Interrupt Key** | EINT0 (P0.1) |
| **RTC Crystal** | 32.768 kHz crystal for RTC module |

---

## ⚙️ Pin Configuration Summary

### 🔹 LCD Connections
| LCD Pin | LPC2148 Pin | Description |
|----------|-------------|-------------|
| D0–D7 | P0.8–P0.15 | Data bus |
| RS | P0.16 | Register select |
| RW | P0.17 | Read/Write |
| EN | P0.18 | Enable |

### 🔹 Keypad (4x4)
| Rows | P1.19–P1.22 | Output pins |
| Cols | P1.16–P1.23 | Input pins |

### 🔹 ADC (LM35)
| Signal | P0.28 (AD0.1) | Analog input channel |

### 🔹 Alarm / LED
| Alarm Output | P0.0 | Buzzer / LED output |

### 🔹 Interrupt
| EINT0 Input | P0.1 | Edit Menu trigger |

---

## 🖥️ Software Flow Summary

### 🕓 Normal Mode
- LCD continuously displays **time, date, day, and temperature**.
- `Check_Alarm()` compares current time with alarm time every loop.

### ⏰ Alarm Mode
- When current time matches alarm:
  - Bell symbol and message are displayed.
  - Buzzer/LED on P0.0 is turned ON.
  - Stops after 60s automatically or immediately on pressing **‘b’ key**.

### 🧭 Edit Menu (Triggered by EINT0)
- Shows options:
SDT -> Set Date/Time

SA -> Set Alarm

EXT -> Exit Menu

yaml
Copy code
- Navigated with keypad symbols:
- `+` → Next field
- `=` → Previous field
- `x` / `-` → Move between lines
- `b` → Save & Exit

---

## 🌡️ Temperature Conversion Logic

```c
adcValue = Read_ADC(1);
voltage = (adcValue * 3.3f) / 1023.0f;
temperature = voltage * 100.0f;   // LM35: 10 mV per °C
Displayed on LCD as:

mathematica
Copy code
27.3°C
🔔 Alarm Display Example
When alarm triggers:

csharp
Copy code
   [Bell Symbol]
Press * 2ExtAlrm*
🧩 Functions Overview
Function	Description
Init_LCD()	Initialize 16x2 LCD in 8-bit mode
Init_KPM()	Setup 4x4 keypad I/O pins
Init_ADC()	Initialize ADC0.1 for LM35 input
RTC_Init()	Initialize RTC with prescaler
SetRTCTimeInfo()	Set time values
SetRTCDateInfo()	Set date values
Set_Alarm()	User sets HH:MM:SS alarm time
Set_DateAlarm()	User sets full date/time
Show_RTC_Display()	Displays time, date, day, temperature
Check_Alarm()	Activates buzzer/LED when alarm matches
Show_Edit_Menu()	Handles external interrupt-driven edit menu

🧾 Key Navigation Summary
Key	Function
+	Move to next digit
=	Move to previous digit
x	Move cursor up
-	Move cursor down
b	Save and exit
*	Exit alarm display
/	(optional for custom use)

🛠️ How to Build and Run
Open Project in Keil µVision / LPCXpresso.

Connect LPC2148 development board.

Compile & Download firmware.

Reset Board — LCD should show date, time, and temperature.

Press EINT0 key (P0.1) to open the edit menu.

🧪 Example Output
makefile
Copy code
19:30:08   27.3°C
18-10-2025  SAT
After pressing EINT0:

markdown
Copy code
****EDIT****
1.SDT 2.SA 3.EXT
📦 Dependencies
ARM7 LPC2148 Microcontroller

Standard ARM C Library

Keil µVision or LPCXpresso IDE

🧑‍💻 Author
Developed by: [Your Name]
📧 Email: [your.email@example.com]
🔗 GitHub: https://github.com/yourusername

🪶 License
This project is open-source under the MIT License — feel free to modify and share.

💡 Tip: If your LCD “blinks” or “shows garbage,” verify your delay timings and contrast potentiometer (VEE).
.

💡 Tip: If your LCD “blinks” or “shows garbage,” verify your delay timings and contrast potentiometer (VEE).
