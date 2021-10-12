#include "includes.h"
#include "func.h"
#include "helpTimer.h"
#include "flash.h"
#include "pid.h"



uint8_t USB_Buf[SH_CL_SIZE];

int calc()
{
  
    g_meas.ready = 0;   
    g_meas.tim1 =0;
    g_meas.tim3 = 0;
    
    
    
    TIM1->CNT = 0; TIM2->CNT = 0;  TIM3->CNT = 0; // start timer
    TIM2->CR1 |= 1; TIM1->CR1 |= 1; TIM3->CR1 |= 1;
   
   
   
   while(!g_meas.ready)
   {
     
   }
   
   TIM1->CR1 &= ~1;
   TIM3->CR1 &= ~1;
   
   unsigned long int  f2 = g_meas.tim3 * 0xFFFF + TIM3->CNT;
   unsigned long int  f1 = g_meas.tim1 * 0xFFFF + TIM1->CNT; 
   
   
   if(!f2)
   {
     g_meas.noSIG = TRUE;
     return 0;
   }
   g_meas.noSIG = FALSE;
      
   
   
   int err =  (long int)f1 - (long int)f2;
   
  
 

   
   return err;
}

void enableClock()
{
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
   
   
}

void init()
{
  initSPI();
  initGPIO();
  initTIM_HAL();
  initTimer();
  initUSART();
  
  enableIRQ();
}

void checkEndCalibr(int err, int dac)
{
  static int cnt = 0;
  
  if(err == 0)
  {
    cnt++;
  }
  else
  {
    cnt = 0;
  }
  
  if(cnt >= 2)
  {
     flash_data_t fl;
     
     fl.dac_val = dac;
     fl.key0 = FLASH_KEY_0;
     fl.key1 = FLASH_KEY_1;
     writeFlash(&fl);
     
     writeLED_READY(1);
     
     
     while(1);
  }
  
}

void main(void)
{
  clockInit();
  
  enableClock();
  init();
  

  
    
  if(checkFlash() != -1) // в есть памяти значение калибровки
  {
    int default_dac;
    flash_data_t fl = readFlash();
    default_dac = fl.dac_val;
       
    writeDAC(default_dac);
  }
  else // в памяти нет значения калибровки
  {
    g_meas.noFLASH = TRUE;
    writeDAC(DAC_NOINF);
  }
  
 
  // ожидание сигнала калибровки на вход
  setTimerLED(TRUE);
  do
  {
    calc();
  }
  while(g_meas.noSIG);
  
  
  
  setTimerLED(FALSE);
  writeLED_SIG(1);
  writeLED_READY(0);

  
  // --- запуск калибровки
  PID_t pid;
  initPID(&pid, DEF_DAC);

  writeDAC(DEF_DAC);

  while(1)
  {
   int err = calc();
   unsigned dac = 0;
   
   inputPID(&pid, err);
   dac = handlePID(&pid);
   writeDAC(dac);
   
   checkEndCalibr(err, dac);
   
   char buff[100] = {0};
   sprintf(buff, "ERR, DAC, %d %d\n\r", err, dac);
   USB_SendBuf(buff, strlen(buff));
   
  }

}
