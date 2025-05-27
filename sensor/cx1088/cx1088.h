#ifndef _CX1088_H_
#define _CX1088_H_

#include <stdio.h>

#define CX1088_ADC_CHANNEL ADC_CHANNEL_11

#ifndef CX1088_PRT
#define CX1088_PRT(...)   \
do {\
    printf("[CX1088_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void cx1088_enable(void);
void cx1088_disable(void);
void cx1088_adc_init(void);
void cx1088_run_once(float *voltage_value);

#endif
