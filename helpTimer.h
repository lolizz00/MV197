#ifndef _HELPTIMER_H_
#define _HELPTIMER_H_

#include <stdint.h>

void clockInit(void);
uint32_t get_TIMCLK1(void);
uint32_t get_TIMCLK3(void);

void enableIRQ();
void __setIRQ(int state);
void initTIM_HAL();

#endif