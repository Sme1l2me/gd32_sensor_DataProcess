#ifndef _AHT20_H_
#define _AHT20_H_

#include <stdio.h>

#define AHT20_SLAVE_ADDRESS   0x70

#ifndef AHT20_PRT
#define AHT20_PRT(...)   \
do {\
    printf("[AHT20_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void aht20_i2c_init(void);
void aht20_run_once(float *humidity, float *temperature);

#endif
