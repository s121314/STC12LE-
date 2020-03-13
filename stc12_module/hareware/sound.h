#ifndef _SOUND_H
#define	_SOUND_H

#include "stc12.h"

sfr P1M0 = 0x92;
sfr P1M1 = 0x91; 

sbit res = P1^7;	//Âö³åÇåÁã¹Ü½Å
sbit pul = P1^6;	//Âö³å½Å
sbit busy = P1^5;	//ĞÅºÅÊä³ö½Å

void reset(void);
void send(uint8_t m);
void YX3P_Init(void);





#endif