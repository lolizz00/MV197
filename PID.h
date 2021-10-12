#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>
#include <math.h>

#define sign(x)  ((signbit(x) ?  -1 : 1))


#include "limitArray.h"
#include "algIncludes.h"


// --------------
#define DAC_MAX 64436
#define DAC_MIN 0

#define MEAN0_CNT 1




// ---------------

typedef struct
{
	uint32_t currDAC;
        
	limit_array_t mean0;

	float currVal;

	BOOL readyFLG;
	BOOL diffReadyFLG;
	

	float oldVal;
	float errorSum;

}PID_t;

BOOL inputPID(PID_t* pid, float val);
uint32_t handlePID(PID_t* pid);
void initPID(PID_t* pid, uint32_t currDAC);
float calcDiff(PID_t* pid, float val);
float calcIntegr(PID_t* pid, float error, float K, float K1);
BOOL checkCorrect(PID_t* pid, float val);
float getMeanError(PID_t* pid);
BOOL meanErrorReady(PID_t* pid);

#endif // !_PID_H_
