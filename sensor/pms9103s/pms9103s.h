#ifndef _PMS9103S_H_
#define _PMS9103S_H_

#include <stdio.h>

#define PMS9103S_UART USART1

#ifndef PMS9103S_PRT
#define PMS9103S_PRT(...)   \
do {\
    printf("[PMS9103S_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void pms9103s_uart_init(void);
void pms9103s_start(void);
void pms9103s_run_once(uint16_t *pm1_0, uint16_t *pm2_5, uint16_t *pm10);

#endif
