#ifndef _LXDGB5A1DPHY_H_
#define _LXDGB5A1DPHY_H_

#include <stdio.h>

#define LXDGB5A1DPHY_ADC_CHANNEL ADC_CHANNEL_1  // 选择 ADC0 采集

#ifndef LXDGB5A1DPHY_PRT
#define LXDGB5A1DPHY_PRT(...)   \
do {\
    printf("[LXDGB5A1DPHY_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void lxdgb5a1dphy_adc_init(void);
void lxdgb5a1dphy_run_once(float *lux);

#endif
