#ifndef _EEPROM_H
#define	_EEPROM_H

#include "stc12.h"

sfr IAP_CONTR = 0xC7;

sfr IAP_DATA    =   0xC2;           //IAP数据寄存器
sfr IAP_ADDRH   =   0xC3;           //IAP地址寄存器高字节
sfr IAP_ADDRL   =   0xC4;           //IAP地址寄存器低字节
sfr IAP_CMD     =   0xC5;           //IAP命令寄存器
sfr IAP_TRIG    =   0xC6;           //IAP命令触发寄存器



#define   ENABLE_IAP  0x82            //if SYSCLK<20MHz
#define 	CMD_READ    1               //IAP字节读命令
#define 	CMD_PROGRAM 2               //IAP字节编程命令
#define 	CMD_ERASE   3               //IAP扇区擦除命令



uint8_t IapReadByte(uint16_t addr);

void IapProgramByte(uint16_t addr, uint8_t dat);

void IapEraseSector(uint16_t addr);

#endif