#ifndef _MSM261S4030H0R_H_
#define _MSM261S4030H0R_H_

#include <stdio.h>

#ifndef MSM261S4030H0R_PRT
#define MSM261S4030H0R_PRT(...)   \
do {\
    printf("[MSM261S4030H0R_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void msm261s4030h0r_init(void);
void msm261s4030h0r_run_once(int *audio_value);

#endif
