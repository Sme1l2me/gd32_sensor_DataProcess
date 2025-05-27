#ifndef _CRD1503G_H_
#define _CRD1503G_H_

#ifndef CRD1503G_PRT
#define CRD1503G_PRT(...)   \
do {\
    printf("[CRD1503G_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

extern volatile uint8_t pulse_ready_flag;
extern volatile uint32_t pulse_count;
extern volatile uint32_t pulse_value;

void crd1503g_timer2_init(void);
void crd1503g_exti_init(void);

#endif
