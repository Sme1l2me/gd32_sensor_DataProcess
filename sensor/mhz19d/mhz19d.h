#ifndef _MHZ19D_H_
#define _MHZ19D_H_

#include <stdio.h>

#define MHZ19D_UART USART0

#ifndef MHZ19D_PRT
#define MHZ19D_PRT(...)   \
do {\
    printf("[MHZ19D_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

extern volatile bool mhz19d_flag;

void mhz19d_uart_init(void);
void mhz19d_hd_disable(void);
void mhz19d_set_range(void);
void mhz19d_run_once(int *co2_ppm);
#endif
