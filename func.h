#ifndef _FUNC_H_
#define _FUNC_H_



typedef struct
{
  volatile unsigned ready;  
  unsigned noSIG;
  unsigned noFLASH;
  
  unsigned tim1;
  unsigned tim3;
}meas_t;

extern meas_t g_meas;

#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "shell.h"
#include "sh_exec.h"
#include "ring_buffer.h"

extern SHELLINFO g_shell;

#define USART_BUFFER_SIZE       (1024)

typedef struct UART_DATA {
  RINGBUFF_T rxBuf;
  RINGBUFF_T txBuf;
} UART_DATA_T;

void initTIM_CCR();
void initTimer();
void startMeas();
void stopMeas();
uint32_t Uart_USB_RecvBuf(uint8_t *Buf, uint32_t BufSize);
void initUSART();
void setTimerLED(int state);
#endif  //_FUNC_H_