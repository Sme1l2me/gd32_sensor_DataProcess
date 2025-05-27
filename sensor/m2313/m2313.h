#ifndef _M2313_H_
#define _M2313_H_

#include <stdio.h>

#define M2313_SLAVE_ADDRESS   0xF0

#ifndef M2313_PRT
#define M2313_PRT(...)   \
do {\
    printf("[M2313_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void m2313_i2c_init(void);
void m2313_run_once(void);

#endif
