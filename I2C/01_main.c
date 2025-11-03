#include <LPC214x.h>
#include "lcd.h"
#include "delay.h"
#include "PLL.h"

#define SLAVE_ADDR 0xA0  // 24C02 write address

//---------------- I2C Functions ----------------//
void I2C0_Init(void)
{
    PINSEL0 |= (1 << 4) | (1 << 6); // P0.2=SCL0, P0.3=SDA0
    I2C0CONCLR = 0x6C;              // Clear all flags
    I2C0SCLH = 300;                 // 100 kHz for 60MHz PCLK
    I2C0SCLL = 300;
    I2C0CONSET = (1 << 6);          // Enable I2C (I2EN)
}

void I2C0_Start(void)
{
    I2C0CONSET = (1 << 5);          // Set STA
    while (!(I2C0CONSET & (1 << 3))); // Wait for SI
    I2C0CONCLR = (1 << 5);          // Clear STA
}

void I2C0_Stop(void)
{
    I2C0CONSET = (1 << 4);          // Set STO
    I2C0CONCLR = (1 << 3);          // Clear SI
}

void I2C0_Write(unsigned char data)
{
    I2C0DAT = data;
    I2C0CONCLR = (1 << 3);          // Clear SI
    while (!(I2C0CONSET & (1 << 3))); // Wait for SI set
}

unsigned char I2C0_Read(void)
{
    I2C0CONSET = (1 << 2);          // Set AA
    I2C0CONCLR = (1 << 3);          // Clear SI
    while (!(I2C0CONSET & (1 << 3))); // Wait for SI
    I2C0CONCLR = (1 << 2);          // Clear AA
    return I2C0DAT;
}

//---------------- Main ----------------//
int main(void)
{
    initPLL();         // PLL = 60 MHz
    I2C0_Init();       // Initialize I2C
    Init_LCD();        // Initialize LCD

    CmdLCD(0x80);
    StrLCD("I2C Write Demo");
    delay_s(1);

    // START + Write sequence
    I2C0_Start();
    I2C0_Write(SLAVE_ADDR);  // Slave address + write
    I2C0_Write(0x00);        // Memory address
    I2C0_Write('A');         // Data byte
    I2C0_Stop();

    delay_ms(10);  // Wait EEPROM write time

    CmdLCD(0xC0);
    StrLCD("Data Sent!");

    while (1);
}
