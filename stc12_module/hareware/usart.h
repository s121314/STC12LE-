#ifndef _USART_H
#define	_USART_H

#include <reg51.h>
#include "stc12.h"



void Usart_Init(uint16_t bound);
void SendData(uint8_t dat);
void SendTwo(uint16_t value);




#endif