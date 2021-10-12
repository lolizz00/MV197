#include "limitArray.h"


void createLimitArray(limit_array_t* arr, uint8_t size)
{
	clearLimitArray(arr);
	arr->size = size;
}

BOOL checkFullLimitArray(limit_array_t* arr)
{
	if (arr->size == arr->currSize)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void addLimitArray(limit_array_t* arr, float val)
{
	if (arr->currSize < arr->size) // добиваем заполнение 
	{
		arr->arr[arr->currSize++] = val;
	}
	else
	{
		arr->arr[arr->_head++] = val;

		if (arr->_head >= arr->size)
		{
			arr->_head = 0;
		}
	}
}

void clearLimitArray(limit_array_t* arr)
{
	memset(arr->arr, 0, sizeof(float) * LIMIT_ARRAY_MAX_SIZE); // !!!!!!!!
	arr->currSize = 0;
	arr->_head = 0;
}

void getPtrLimitArray(limit_array_t* arr, float** ptr0, float** ptr1, uint32_t* ptr0Len, uint32_t* ptr1Len)
{
	*ptr0 = &arr->arr[arr->_head];
	*ptr0Len = arr->size - arr->_head;

	*ptr1 = &arr->arr[0];
	*ptr1Len = arr->_head;
}

float calcMean(limit_array_t* arr)
{
	double res = 0;

	for(int i =0; i < arr->size;i++)
	{
		res += arr->arr[i];
	}
	
	res = res / arr->size;


	return res;


}

