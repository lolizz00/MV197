#ifndef _FLASH_H_
#define _FLASH_H_

#define PARAMS_ADDR ((void*)0x0801F800)

#define FLASH_KEY_0 0x1F
#define FLASH_KEY_1 0xB1

#define DEF_DAC   22200
#define DAC_NOINF 0x7FFF

typedef struct
{
  unsigned key0;
  
  unsigned dac_val;
  
  unsigned key1;
  
}flash_data_t;
void writeFLASH_DAC( unsigned dac);

flash_data_t readFlash();
void writeFlash(flash_data_t*);
int checkFlash();
void writeDefFLASH();


#endif