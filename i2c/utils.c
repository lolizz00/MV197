#include "includes.h"


void initGPIO()
{
   GPIO_InitTypeDef port;
   port.GPIO_Speed = GPIO_Speed_50MHz;
   port.GPIO_Mode =  GPIO_Mode_Out_PP ;
   
   port.GPIO_Pin = GPIO_Pin_2;
   GPIO_Init(GPIOC, &port);   
   
   port.GPIO_Pin = GPIO_Pin_1;
   GPIO_Init(GPIOC, &port); 
  
}

void writeLED_SIG(int state)
{
  if(state)
  {
   GPIO_WriteBit(GPIOC,  GPIO_Pin_1, Bit_SET);
  }
  else
  {
     GPIO_WriteBit(GPIOC,  GPIO_Pin_1, Bit_RESET);
  }
}

void writeLED_READY(int state)
{
   if(state)
  {
   GPIO_WriteBit(GPIOC,  GPIO_Pin_2, Bit_SET);
  }
  else
  {
     GPIO_WriteBit(GPIOC,  GPIO_Pin_2, Bit_RESET);
  }
}


char *strcat_s(char * s1, int numberOfElements, const char * s2)
{
  if (strlen(s1) + strlen(s2) < numberOfElements)
  {
    strcat(s1, s2);
  }
  else
  {
    int freespace;
    freespace = numberOfElements - strlen(s1) - 1;
    strncpy(&s1[strlen(s1)], s2, freespace);
  }
  return s1;
}

void delay_ms(uint32_t ms)
{
  volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);
  nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
  for (; nCount!=0; nCount--);
}


  
