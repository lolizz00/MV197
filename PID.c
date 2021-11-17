#include "PID.h"

int _abs(int val)
{
  if(val > 0)
  {
    return val;
  }
  else
  {
    return -val;
  }
}

float getMeanError(PID_t* pid)
{
	// фактически, получаем усредненную ошибку
	float mean = calcMean(&pid->mean0);
	return mean;
}

BOOL meanErrorReady(PID_t* pid)
{
	return checkFullLimitArray(&pid->mean0);
}

BOOL checkCorrect(PID_t* pid, float val)
{
  
 /* static BOOL firstFLG = TRUE;
  
  if(firstFLG)
  {
    firstFLG = FALSE;
    return FALSE;
  }*/
        
  return TRUE;
  
	
}


BOOL inputPID(PID_t* pid, float val)
{
	
	if (checkCorrect(pid, val))
	{
		addLimitArray(&pid->mean0, val);

		if (checkFullLimitArray(&pid->mean0))
		{
                        float mean = calcMean(&pid->mean0);
			pid->readyFLG = TRUE;
			pid->currVal = mean;

		}
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
	
	
}

uint32_t handlePID(PID_t* pid, int* diff)
{
	if (!pid->readyFLG)
	{
		return pid->currDAC;  
	}

	int error = (int)pid->currVal;

	float up = 0;
	float ud = 0;
	float ui = 0;
        
        if (_abs(error) < 2)
	{
          up = error * 2;
        }
        else if (_abs(error) <= 6)
	{
          up = error * 30;
        }
        else
        {
           up = error * 50;
        }
        
        /*if(diff)
        {
          *diff = round(up + ui + ud);
        }*/
        
	
	pid->currDAC += round(up + ui + ud);
	
	return pid->currDAC;


}

void initPID(PID_t* pid, uint32_t currDAC)
{

	pid->currDAC = currDAC;
	pid->currVal = 0;

	createLimitArray(&pid->mean0, MEAN0_CNT);
	
        
	pid->readyFLG = FALSE;

	pid->diffReadyFLG = FALSE;

	pid->oldVal = 0;
	pid->errorSum = 0;


}


float calcDiff(PID_t* pid, float val)
{
	float res = 0;

	res = val - pid->oldVal;
	pid->oldVal = val;

	if (!pid->diffReadyFLG)
	{
		pid->diffReadyFLG = TRUE;
		return 0;
	}

	return -res;
}

float calcIntegr(PID_t* pid, float error, float K, float K1)
{
	pid->errorSum = pid->errorSum + error;

	if (pid->errorSum < -K1)
	{
		pid->errorSum = 0;
		return K;
	}

	if (pid->errorSum > K1)
	{
		pid->errorSum = 0;
		return -K;
	}

	return 0;

}