#ifndef _LIMIT_ARRAY_H_
#define _LIMIT_ARRAY_H_

#include <stdint.h>

#include "algIncludes.h"

#define LIMIT_ARRAY_MAX_SIZE 100



typedef struct 
{
	uint8_t size; // размер массива
	uint8_t currSize; // текущий размер
	float arr[LIMIT_ARRAY_MAX_SIZE];
	uint8_t _head;
}limit_array_t;


void createLimitArray(limit_array_t* arr, uint8_t size);
BOOL checkFullLimitArray(limit_array_t* arr);
void addLimitArray(limit_array_t* arr, float val);
void clearLimitArray(limit_array_t* arr);
void getPtrLimitArray(limit_array_t* arr, float** ptr0, float** ptr1, uint32_t* ptr0Len, uint32_t* ptr1Len);
float calcMean(limit_array_t* arr);

#endif // !_LIMIT_ARRAY_
