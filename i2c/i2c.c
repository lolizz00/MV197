#include "i2c.h"

uint32_t Timeout = 0; 


//#define SPI_SOFT

#define SPI_PORT GPIOA

#define SPI_CLK GPIO_Pin_5
#define SPI_DATA GPIO_Pin_7
#define _SPI_NSS GPIO_Pin_4


#pragma optimize=low
void initSPI()
{
 
   GPIO_InitTypeDef port;
   port.GPIO_Mode = GPIO_Mode_Out_PP;
   port.GPIO_Speed = GPIO_Speed_50MHz;
   
   port.GPIO_Pin = _SPI_NSS;
   GPIO_Init(GPIOA, &port);
   GPIO_WriteBit(SPI_PORT,  _SPI_NSS,  Bit_SET);
   
   
   
#ifdef SPI_SOFT
    port.GPIO_Pin = SPI_DATA;
    GPIO_Init(GPIOA, &port);
    
    port.GPIO_Pin = SPI_CLK;
    GPIO_Init(GPIOA, &port);
    
    GPIO_WriteBit(SPI_PORT,  SPI_DATA, Bit_RESET);
    GPIO_WriteBit(SPI_PORT,  SPI_CLK,  Bit_SET);
#else
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    port.GPIO_Mode =  GPIO_Mode_AF_PP;
    
    port.GPIO_Pin = SPI_DATA;
    GPIO_Init(GPIOA, &port);
    
    port.GPIO_Pin = SPI_CLK;
    GPIO_Init(GPIOA, &port);
    
    port.GPIO_Pin =  GPIO_Pin_6;
    GPIO_Init(GPIOA, &port);
    
    SPI_InitTypeDef spi;
   
    spi.SPI_CPOL =  SPI_CPOL_Low;
    spi.SPI_CPHA =  SPI_CPHA_2Edge;
   
    
    spi.SPI_Direction =  SPI_Direction_1Line_Tx;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
   
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    spi.SPI_NSS = SPI_NSS_Soft ;
    SPI_Init(SPI1, &spi);
       
   
    
    SPI_Cmd(SPI1, ENABLE);
    
#endif      
   
  
      
   
  
}



#pragma optimize=low
int _writeDAC(uint16_t val)
{
  
  const uint8_t addr = 0x0;

  uint8_t byte1 = (val & 0xFF00) >> 8;
  uint8_t byte2 = (val & 0xFF);

  uint8_t dataBuf[3] = {addr, byte1, byte2} ;
  
  GPIO_WriteBit(SPI_PORT,  _SPI_NSS, Bit_RESET);
  
#ifdef SPI_SOFT  
  for (int i = 0; i<=2; i++)
  {
	for (int j=7; j>=0; j--)
        {
          GPIO_WriteBit(SPI_PORT,  SPI_DATA, (dataBuf[i] >> j) & 1);
          
	  GPIO_WriteBit(SPI_PORT,  SPI_CLK,  Bit_RESET);
          delay_ms(1);
	  GPIO_WriteBit(SPI_PORT,  SPI_CLK,  Bit_SET);
	}
   }
#else
  for (int i = 0; i<=2; i++)
  {
    SPI_SendData(SPI1, dataBuf[i]);
    Timed(!SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE));
    SPI_ClearFlag(SPI1, SPI_FLAG_TXE);
  }
#endif
  
   
   GPIO_WriteBit(SPI_PORT,  _SPI_NSS, Bit_SET);
   
  
  

  
  /*GPIO_WriteBit(GPIOA,GPIO_Pin_4, Bit_RESET);
  SPI_SendData(SPI1, addr);
  SPI_SendData(SPI1, byte1);
  SPI_SendData(SPI1, byte2);
  GPIO_WriteBit(GPIOA,GPIO_Pin_4, Bit_SET);*/
  
  return 0;
  
errReturn:
  return - 1;
  

  
  
}

int writeDAC(uint16_t val)
{
  for(int i =0; i < 3; i++)
  {
    _writeDAC(val);
  }
}