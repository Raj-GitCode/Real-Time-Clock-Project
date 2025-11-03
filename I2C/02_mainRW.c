#include <LPC214x.h>
#include "lcd.h"
#include "delay.h"
#include "PLL.h"

#define SLAVE_ADDR_WRITE 0xA0
#define SLAVE_ADDR_READ  0xA1

//------------------------------------------------
// I²C Core Functions
//------------------------------------------------
void I2C0_Init(void)
{
    PINSEL0 |= (1 << 4) | (1 << 6); // P0.2=SCL0, P0.3=SDA0
    I2C0CONCLR = 0x6C;              // Clear all flags
    I2C0SCLH = 300;                 // 100 kHz @ 60 MHz PCLK
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
    I2C0CONSET = (1 << 2);          // Set AA to ACK
    I2C0CONCLR = (1 << 3);          // Clear SI
    while (!(I2C0CONSET & (1 << 3))); // Wait SI
    I2C0CONCLR = (1 << 2);          // Clear AA
    return I2C0DAT;
}

//------------------------------------------------
// EEPROM Helper Functions
//------------------------------------------------
void I2C_WriteByte(unsigned char mem_addr, unsigned char data)
{
    I2C0_Start();
    I2C0_Write(SLAVE_ADDR_WRITE);  // SLA+W
    I2C0_Write(mem_addr);          // Memory address
    I2C0_Write(data);              // Data byte
    I2C0_Stop();
    delay_ms(10);                  // EEPROM write time
}

unsigned char I2C_ReadByte(unsigned char mem_addr)
{
    unsigned char data;

    I2C0_Start();
    I2C0_Write(SLAVE_ADDR_WRITE);  // SLA+W
    I2C0_Write(mem_addr);          // Memory address
    I2C0_Start();                  // Repeated Start
    I2C0_Write(SLAVE_ADDR_READ);   // SLA+R
    data = I2C0_Read();            // Read one byte
    I2C0_Stop();

    return data;
}

//------------------------------------------------
// Main Program
//------------------------------------------------
int main(void)
{
    unsigned char data;

    initPLL();
    I2C0_Init();
    Init_LCD();

    CmdLCD(0x80);
    StrLCD("I2C EEPROM Demo");

    // ---- Write ----
    I2C_WriteByte(0x00, 'A');

    // ---- Read ----
    data = I2C_ReadByte(0x00);

    // ---- Display ----
    CmdLCD(0xC0);
    StrLCD("Read: ");
    CharLCD(data);

    while (1);
}
