#include "helpTimer.h"
#include "func.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim1.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"



#include "system_stm32f10x.h"

meas_t g_meas;




SHELLINFO g_shell;


UART_DATA_T g_UART_USB;
uint8_t UART_USB_RxBuffer[USART_BUFFER_SIZE];
uint8_t UART_USB_TxBuffer[USART_BUFFER_SIZE];


uint32_t Uart_USB_RecvBuf(uint8_t *Buf, uint32_t BufSize)
{
  uint32_t count = 0;
  if ((count = RingBuffer_GetCount(&g_UART_USB.rxBuf)) > 0)
  {
    
     if(count > BufSize)
     {
          count = BufSize;
     }
    
    count = RingBuffer_PopMult(&g_UART_USB.rxBuf, Buf, count); 
  }
  return count;
}

void Uart_USB_Init(USART_InitTypeDef* USART_InitStruct)
{
   //RingBuffer_Init(&g_UART_USB.rxBuf, UART_USB_RxBuffer, 1, USART_BUFFER_SIZE);
  // RingBuffer_Init(&g_UART_USB.txBuf, UART_USB_TxBuffer, 1, USART_BUFFER_SIZE);
   
  
   GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

   GPIO_InitTypeDef port;
   port.GPIO_Speed = GPIO_Speed_50MHz;
   
   port.GPIO_Mode =  GPIO_Mode_AF_PP ;
   port.GPIO_Pin = GPIO_Pin_6;
   GPIO_Init(GPIOB, &port);   
   
   port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   port.GPIO_Pin = GPIO_Pin_7;
   GPIO_Init(GPIOB, &port); 

   
   USART_Init(USART1, USART_InitStruct);
   
   USART_Cmd(USART1, ENABLE);
   
  // USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    
}

void initUSART()
{
  
  
  
  USART_InitTypeDef USART_InitStructure;
  USART_StructInit(&USART_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 57600;  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


  //RingBuffer_Init(&g_UART_USB.rxBuf, UART_USB_RxBuffer, 1, USART_BUFFER_SIZE);
 // RingBuffer_Init(&g_UART_USB.txBuf, UART_USB_TxBuffer, 1, USART_BUFFER_SIZE);
  
  Uart_USB_Init(&USART_InitStructure);
  
  //sh_exec_init(&g_shell);
}



#pragma optimize=low
void initTIM_CCR()
{
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
   
   
   
   
   GPIO_InitTypeDef port;
   GPIO_StructInit(&port);
   port.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
   port.GPIO_Speed = GPIO_Speed_50MHz;
   port.GPIO_Pin = GPIO_Pin_9 ;
   GPIO_Init(GPIOA, &port);   
     
   TIM1_TimeBaseInitTypeDef timer_base;
   TIM1_TimeBaseStructInit(&timer_base);
   timer_base.TIM1_Prescaler = 0;
   TIM1_TimeBaseInit(&timer_base);
   
   
   TIM1_ICInitTypeDef timer;   
   timer.TIM1_ICFilter = 0;
   timer.TIM1_ICPolarity = TIM1_ICPolarity_Falling;
   timer.TIM1_ICPrescaler = TIM1_ICPSC_DIV8;
   timer.TIM1_ICSelection=  TIM1_ICSelection_DirectTI;
   
   
   timer.TIM1_Channel = TIM1_Channel_1;
   TIM1_ICInit(&timer);
  
   timer.TIM1_Channel = TIM1_Channel_2;
   TIM1_ICInit(&timer);
    
   TIM1_ARRPreloadConfig(ENABLE);
     
  
   TIM1_Cmd(ENABLE);
  
   TIM1->EGR = 0x6;
   
  // TIM1_ITConfig(TIM1_IT_CC1, ENABLE);
  // TIM1_ITConfig(TIM1_IT_CC2, ENABLE);
   
   
   TIM1_CCxCmd(TIM1_Channel_1,  ENABLE);
   TIM1_CCxCmd(TIM1_Channel_2 ,ENABLE);
}

#pragma optimize=low
static void initTIM2()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
  
  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);
  
  base_timer.TIM_Prescaler = 0xFFFF;
  base_timer.TIM_Period =  0xFFF1;
  TIM_TimeBaseInit(TIM2, &base_timer);
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_ARRPreloadConfig(TIM2, DISABLE);
   
   
}

void setTimerLED(int state)
{
  if(state)
  {
     TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  }
  else
  {
     TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
  }
}

void initTimerLED()
{
  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);
  
  base_timer.TIM_Prescaler = 0xFFFF;
  base_timer.TIM_Period =  1000;
  TIM_TimeBaseInit(TIM4, &base_timer);
  
  
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  
  TIM4->CR1 |= 1;
  
}

void initTimer()
{   
  initTIM2();  
  initTimerLED();
}


extern unsigned sig_sch;
extern unsigned et_sch;


#pragma optimize=low
void startMeas()
{
    
   g_meas.ready = 0;

   
   et_sch = 0;
   sig_sch = 0;
   
   //TIM3->CNT = 0;
   //TIM_Cmd(TIM3, ENABLE);
   
      
  
   __setIRQ(1);
   
    
}




#pragma optimize=low
void stopMeas()
{
   
 // et_sch = 0;
 // sig_sch = 0;
    
   
  __setIRQ(0);
       
}



int checkMeasReady()
{
  return g_meas.ready;
}

int getErrorMeas()
{
  return 0;
}

int getNoSIG()
{
  return g_meas.noSIG;
}









