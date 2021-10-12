#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "stm32f10x_conf.h"

#define BOOL      uint8_t


#define Timed(x) Timeout = 0xFFFF; while (x) { if (Timeout-- == 0) goto errReturn;} 

void writeLED_SIG(int state);
void writeLED_READY(int state);

void delay_ms(uint32_t ms);


char *strcat_s(char * s1, int numberOfElements, const char * s2);


// ���� ����� ������ ������

void initGPIO();

#endif // _UTILS_H_