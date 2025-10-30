//adc_defines.h
#ifndef ADC_DEFINES_H
#define ADC_DEFINES_H

// System clocks
#define FOSC 12000000
#define CCLK (FOSC * 5)       // 60 MHz
#define PCLK (CCLK / 4)       // 15 MHz
#define ADCCLK 3000000
#define CLKDIV ((PCLK / ADCCLK) - 1)

// ADC register bit defines
#define CLKDIV_BITS    8
#define PDN_BIT        21
#define START_BIT      24
#define RESULT_BITS     6
#define DONE_BIT       31

// ADC channel mapping
#define ADC0_PINSEL   (1 << 24)   // P0.28 -> AD0.1

#endif
