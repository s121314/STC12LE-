#ifndef _KEY_H
#define	_KEY_H

#include "stc12.h"


sfr P4SW = 0xBB;


sbit K_S3 = P0^7;		
sbit K_S2 = P4^6;	
sbit POWERDOWN = P0^5;
sbit K_S1 = P0^6;

#define False 0
#define True 1	

#define	KEY_1			0x01
#define	KEY_2 		0x02
#define	KEY_3			0x03
#define	Key_None  0x00


#define	KEY_TOTAL 	0x05
#define	LONG_PRESS_KEY1 	(KEY_1 + KEY_TOTAL)


void Key_Init(void);
void key_scan(void);
void key_driver(void);


#endif