
#include "other.h"


void Shake_Init(void)
{
	P4M0 |= (1 << 4);
	P4M1 &= ~(1 << 4);	//P4.4 ÅäÖÃÎªÊä³ö

	Shake_Pin(0);
}