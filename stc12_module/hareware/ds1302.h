#ifndef _DS1302_H
#define	_DS1302_H

#include "stc12.h"



sbit Sclk = P2^7;		//SCLK
sbit Dio = P2^6;		//DIO
sbit Ce  = P2^5;		//RST

sbit ACC_7 = ACC^7;

#define Write_Seond		0x80
#define Write_Minute	0x82
#define Write_Hour		0x84
#define Write_Day		0x86
#define Write_Month		0x88
#define Write_Year		0x8c


#define Read_Seond	0x81
#define Read_Minute	0x83
#define Read_Hour	0x85
#define Read_Day	0x87
#define Read_Month	0x89
#define Read_Year	0x8d

#define Write_Protect	0x8e

void Set1302(void);
void Update_DateAndTime(void);
//void Init_Ds1302(void);
void Ds1302_Init(void);


#endif