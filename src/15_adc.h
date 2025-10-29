//adc.h
#ifndef ADC_H
#define ADC_H

#include "types.h"

// ADC Initialization
void Init_ADC(void);

// Read ADC channel (0–7), returns 10-bit value
unsigned int Read_ADC(unsigned int ch);

#endif
