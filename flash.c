#include "flash.h"

#include <string.h>
#include "stm32f10x_flash.h"

flash_data_t readFlash()
{
  
  flash_data_t flash;
  memcpy(&flash, PARAMS_ADDR , sizeof(flash_data_t));
  return flash;
}
         
void writeFlash(flash_data_t* flash)
{
   FLASH_Unlock();
   FLASH_ErasePage((unsigned)PARAMS_ADDR);
   
   unsigned* ptr = (unsigned*)flash;
   
   for(int i =0; i < sizeof(flash_data_t); i = i + 4)
   {
     unsigned data = *(ptr + (i/4));
     FLASH_ProgramWord( (unsigned)PARAMS_ADDR + i,  data);
   }
  
   FLASH_Lock();

}



void writeDefFLASH()
{
   flash_data_t flash;
   flash.dac_val = DEF_DAC;
   flash.key0 = FLASH_KEY_0;
   flash.key1 = FLASH_KEY_1;
   
   writeFlash(&flash);
}
         
int checkFlash()
{
  flash_data_t data = readFlash();
  
  
  if((data.key0 == FLASH_KEY_0) && (data.key1 == FLASH_KEY_1))
  {
    return 0;
  }
  else
  {
    return -1;
  }
  
}