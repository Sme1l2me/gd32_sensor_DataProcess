#ifndef _JXO2101_H_
#define _JXO2101_H_

#include <stdio.h>

#ifndef JXO2101_PRT
#define JXO2101_PRT(...)   \
do {\
    printf("[JXO2101_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

extern volatile bool jxo2101_flag;

void jxo2101_uart_init(void);
void jxo2101_run_once(float *o2_value);

#endif
