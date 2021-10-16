#include "includes.h"
#include "func.h"
#include "helpTimer.h"
#include "flash.h"
#include "pid.h"

extern const char version_logo[];

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
   
   
   if(f2 < 10)
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
     writeFLASH_DAC( dac);
     
     writeLED_READY(1);
     
     printf("Calibration finished!\n\r");
     
     while(1);
  }
  
}

void main(void)
{
  clockInit();
  
  enableClock();
  init();
  
  for(int i =0; i < 0xFFF;i++);
  
  printf(" Software verion: %s\n\r", version_logo);
  
    
  if(checkFlash() != -1) // в есть памяти значение калибровки
  {
    int default_dac;
    flash_data_t fl = readFlash();
    default_dac = fl.dac_val;
       
    writeDAC(default_dac);
    
    printf("Calibration value read successfully.\n\r");
    
  }
  else // в памяти нет значения калибровки
  {
    g_meas.noFLASH = TRUE;
    writeDAC(DAC_NOINF);
    printf("Failed to read the calibration value.\n\r");
    
  }
  
 
  // ожидание сигнала калибровки на вход
  
  
  TIM2->ARR = 0xFFF; // временное значение, для быстрого обнаружения сигнала
  
  setTimerLED(TRUE);
  do
  {
    calc();
  }
  while(g_meas.noSIG);
  
  TIM2->ARR = 0xFFF1; // вовзвращаем нормальное значение
  
  
  setTimerLED(FALSE);
  writeLED_SIG(1);
  writeLED_READY(0);

  printf("Signal 10MHz detected. Calibration started...\n\r");
   
  // --- запуск калибровки
  PID_t pid;
  initPID(&pid, DEF_DAC);

  writeDAC(DEF_DAC);

  while(1)
  {
   int err = calc();
   unsigned dac = 0;
   int diff;
   
   inputPID(&pid, err);
   dac = handlePID(&pid, &diff);
   writeDAC(dac);
   
   writeFLASH_DAC(dac);
   
   checkEndCalibr(err, dac);
   
   printf("Error, DAC value, DAC shift: %d\t%d\t%d\n\r", err, dac, diff);
   printf("Value saved to FLASH.\n\r");
  }

}
