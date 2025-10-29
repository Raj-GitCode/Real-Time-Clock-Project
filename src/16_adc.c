//adc.c
#include <LPC214x.h>
#include "adc.h"
#include "adc_defines.h"

void Init_ADC(void)
{
    // Configure P0.27 as AD0.0 input
    PINSEL1 |= ADC0_PINSEL;

    // Power up ADC and set clock divider
    AD0CR = (1 << PDN_BIT) | (CLKDIV << CLKDIV_BITS);
}

unsigned int Read_ADC(unsigned int ch)
{
    unsigned int result;

    // Clear previous channel selection (bits 0–7)
    AD0CR &= 0xFFFFFF00;
    AD0CR |= (1 << ch);             // Select channel
    AD0CR |= (1 << START_BIT);      // Start conversion

    // Wait until conversion is done
    while (((AD0GDR >> DONE_BIT) & 1) == 0);

    // Extract 10-bit result
    result = (AD0GDR >> RESULT_BITS) & 0x3FF;

    // Stop conversion (clear START bits 24-26)
    AD0CR &= ~(0x07 << 24);

    return result;
}
