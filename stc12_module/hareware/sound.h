#ifndef _SOUND_H
#define	_SOUND_H

#include "stc12.h"

sfr P1M0 = 0x92;
sfr P1M1 = 0x91; 

sbit res = P1^7;	//��������ܽ�
sbit pul = P1^6;	//�����
sbit busy = P1^5;	//�ź������

void reset(void);
void send(uint8_t m);
void YX3P_Init(void);





#endif