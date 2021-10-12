#include "helpTimer.h"
#include "stm32f1xx_hal.h"

void _Error_Handler(char *file, int line)
{
  
  while(1)
  {
    asm("nop");
  }
 
}


TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;


#define __PRE      0
#define __TRIG_PRE TIM_TRIGGERPRESCALER_DIV1
#define __DIV      TIM_CLOCKDIVISION_DIV1



/* TIM1 init function */
#pragma optimize=low
static void MX_TIM1_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = __PRE;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xFFFF;
  htim1.Init.ClockDivision = __DIV;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = __TRIG_PRE;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchronization(&htim1, &sSlaveConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}



/* TIM3 init function */
#pragma optimize=low
static void MX_TIM3_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = __PRE;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xFFFF;
  htim3.Init.ClockDivision = __DIV;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = __TRIG_PRE;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchronization(&htim3, &sSlaveConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;


  /*Configure GPIO pins : PA5 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8 |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT ;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


uint32_t get_TIMCLK3(void) 
{
  /* Get PCLK1 frequency */
  uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
 
  /* Get PCLK1 prescaler */
  if((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
  {
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return (pclk1);
  }
  else
  {
    /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return(2 * pclk1);
  }
}

uint32_t get_TIMCLK1(void) 
{
  
 uint32_t pclk1 = HAL_RCC_GetSysClockFreq();
  /* Get PCLK1 frequency */
   pclk1 = HAL_RCC_GetPCLK2Freq();
 
  /* Get PCLK1 prescaler */
  if((RCC->CFGR & RCC_CFGR_PPRE2) == 0)
  {
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return (pclk1);
  }
  else
  {
    /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return(2 * pclk1);
  }
}

#pragma optimize=low
void _SystemInit (void)
{
   RCC->CR |= 0x00000001U;
   RCC->CR &= 0xFEF6FFFFU;
   RCC->CR &= 0xFFFBFFFFU;
   RCC->CFGR &= 0xFF80FFFFU;
}

#pragma optimize=low
void clockInit(void)
{
  _SystemInit();
  
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
 
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;


  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  
  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  
  // --- 
  
  

}


void initTIM_HAL()
{
   
   //MX_GPIO_Init();
   MX_TIM1_Init();
   MX_TIM3_Init();
   
    TIM1->DIER |= TIM_DIER_UIE;
    TIM3->DIER |= TIM_DIER_UIE;
}


void __setIRQ(int state)
{
  if(state)
  {
   // NVIC_EnableIRQ(TIM3_IRQn    );
  }
  else
  {
   // NVIC_DisableIRQ(TIM1_CC_IRQn);
  }
}

#pragma optimize=low
void enableIRQ()
{
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   
    
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);

    
    
    
   __enable_irq();
}





