#ifndef _OTHER_H
#define	_OTHER_H

#include "stc12.h"


sbit P44 = P4^4;


#define	 Shake_Pin(x)  (P44 = x)



void Shake_Init(void);

#endif