#ifndef _I2C_H_
#define _I2C_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

#include "utils.h"
#include "shell.h"

#include <stdint.h>

void initSPI();
int writeDAC(uint16_t val);


#endif // _I2C_H_