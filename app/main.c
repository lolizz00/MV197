#include "includes.h"
#include "func.h"
#include "helpTimer.h"
#include "flash.h"
#include "pid.h"

extern const char version_logo[];

uint8_t USB_Buf[SH_CL_SIZE];

int calcForReady()
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
   
   unsigned long int  f1 = g_meas.tim3 * 0xFFFF + TIM3->CNT;
   unsigned long int  f2 = g_meas.tim1 * 0xFFFF + TIM1->CNT; 
   
   
   
   
   if( ((f2 < 40000000) || (f2 > 45000000)) || ((f1 < 40000000) || (f1 > 45000000)))
   {
     g_meas.noSIG = TRUE;
     return 0;
   }
   g_meas.noSIG = FALSE;
      
   
   
   int err =  (long int)f1 - (long int)f2;
   
  
 

   
   return err;
}

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
   
   unsigned long int  f1 = g_meas.tim3 * 0xFFFF + TIM3->CNT;
   unsigned long int  f2 = g_meas.tim1 * 0xFFFF + TIM1->CNT; 
   
   
   
   
   if( (f2 < 10) || (f1 < 10) )
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

// ожидание сигнала на вход
void waitSIG()
{
 // TIM2->ARR = 0xFFF; // временное значение, для быстрого обнаружения сигнала
  
  setTimerLED(TRUE);
  do
  {
    calcForReady();
  }
  while(g_meas.noSIG);
  
 // TIM2->ARR = 0xFFF1; // вовзвращаем нормальное значение
}

// ожидание прогрева
void waitReadyOCXO()
{
  
   static long int l_val = 0;
  
    
    while(1)
    {
   

      int error = calcForReady();
           
      if(abs(error - l_val) < 2)
      {
        return;
      }
      else
      {
        l_val = error;
      }
      
   }

}


// поебота для проверки того, что все скалибровано
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
  
  if(cnt >= 1)
  {
     writeFLASH_DAC( dac);
     
     writeLED_READY(1);
     
     printf("Calibration finished!\n\r");
     
     while(1);
  }
  
}


#define FISRT_STEP_DIFF_DAC 10000
#define N_MID 3
int firstStep(unsigned default_dac)
{
  
  int arr[N_MID]; 
  int error_2 =0;
  int error_1 =0;
  
  writeDAC(default_dac - FISRT_STEP_DIFF_DAC);
  for(int i =0; i < N_MID;i++) { arr[i] = calc(); }
  for(int i =0; i < N_MID;i++) { error_2 += arr[i];}
  error_2 = error_2 / N_MID;
  

  writeDAC(default_dac);
  for(int i =0; i < N_MID;i++) { arr[i] = calc(); }
  for(int i =0; i < N_MID;i++) { error_1 += arr[i];}
  error_1 = error_1 / N_MID;
  
  float k = (float)abs(error_1 - error_2) / (float)FISRT_STEP_DIFF_DAC; 
  // хуйня для 1 у.е.
  
  int res =  -((float)error_1 / k);
  return res;
  
  
}


void main(void)
{
  clockInit();
  enableClock();
  init();
  
  
  for(int i =0; i < 0xFFF;i++);
  
  printf(" Software verion: %s\n\r", version_logo);
  
  int default_dac;
  if(checkFlash() != -1) // в есть памяти значение калибровки
  {
    
    flash_data_t fl = readFlash();
    default_dac = fl.dac_val;
       
    writeDAC(default_dac);
    
    printf("Calibration value read successfully.\n\r");
    
  }
  else // в памяти нет значения калибровки
  {
    g_meas.noFLASH = TRUE;
    writeDAC(DAC_NOINF);
    default_dac = DAC_NOINF;
    printf("Failed to read the calibration value.\n\r");
  }
  
  TIM2->ARR = 0xFFF; // временное значение, для быстрого обнаружения сигнала
  
  
  
 
  // ожидание сигнала калибровки на вход
  printf("Wait for input signal 10MHz..\n\r");
  waitSIG();
  
   
  
 
  
  
  //ждем прогрева
  printf("Signal 10MHz detected. Wait for OCXO ready..\n\r");
  waitReadyOCXO();
  
  setTimerLED(FALSE);
  writeLED_SIG(1);
  writeLED_READY(0);
  
  
  printf("Calculations for the first step started.\n\r");
  int k = firstStep(default_dac);
  
  printf("New DAC = %u , k = %d\n\r", default_dac -k, k);
  
  default_dac -= k;
  printf("The first step is done.\n\r");
 
  
  TIM2->ARR = 0xFFE0; // вовзвращаем нормальное значение
  
  // --- запуск калибровки
  PID_t pid;
  initPID(&pid, default_dac);

  writeDAC(default_dac);
  writeFLASH_DAC(default_dac);
  
  
  printf("Calibration started.\n\r");
  
  
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
