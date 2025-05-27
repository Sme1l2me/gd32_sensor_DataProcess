#ifndef _E_R_SENSOR_H_
#define _E_R_SENSOR_H_

#include <stdio.h>

#define ERSENSOR_ADC_CHANNEL ADC_CHANNEL_10

#ifndef ERSENSOR_PRT
#define ERSENSOR_PRT(...)   \
do {\
    printf("[ERSENSOR_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void ersensor_adc_init(void);
void ersensor_run_once(float *radiation_voltage_value);

#endif
