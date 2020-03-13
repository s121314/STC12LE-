
#include "sound.h"


void YX3P_Init(void)
{
	P1M0 |= (3 << 6);
	P1M1 &= ~(3 << 6);	 //pulse 和res 配置为输出

	P1M0 &= ~(1 << 5);
	P1M1 |= (1 << 5);	//busy 配置为输入
}


void reset(void)
{
	pul = 0;
	res = 0;
	Delay(10);
	res = 1;
	Delay(10);
	res = 0;
	Delay(10);
}
	
void send(uint8_t m)
{
	for(;m>0;m--)
	{
		pul = 0;
		Delay(10);
		pul = 1;
		Delay(10);
	}
	pul = 0;
	Delay(10);
}








