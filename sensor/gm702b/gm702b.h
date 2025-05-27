#ifndef _GM702B_H_
#define _GM702B_H_

#include <stdio.h>

#define ADC_CHANNEL_SENSOR ADC_CHANNEL_0  // 选择 ADC0 采集 RS2

#ifndef GM702B_PRT
#define GM702B_PRT(...)   \
do {\
    printf("[GM702B_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void gm702b_adc_init(void);
void gm702b_warm_up(void);
void gm702b_run_once(float *COppm);

#endif
