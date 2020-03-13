

#include "ds1302.h"




uint8_t Seond = 10;
uint8_t Minute = 20;
uint8_t Hour= 0;	
uint8_t Day= 0;	
uint8_t Month= 0;
uint8_t Year= 0;



//p2.6 -->IO
//p2.5 -->CE
//p2.7 -->sclk
void Ds1302_Init(void)
{
	P2M1 |= 7 << 5;
	P2M0 |= 7 << 5;	//��©���
}


void Write1302(uint8_t addr,uint8_t dat)
{
	uint8_t i, temp;
	
	Ce = 0;
	Sclk = 0;
	Ce = 1;

	//send addr
	temp=addr;
	for(i=8;i>0;i--)
	{
		Sclk = 0;
		Delay(1);
		Dio = (bit)(temp&0x01);
		Delay(1);
		temp>>=1;
		Sclk = 1;
		Delay(1);
	}

	//send dat
	temp=dat;
	for(i=8;i>0;i--)
	{
		Sclk = 0;
		Delay(1);
		Dio = (bit)(temp&0x01);
		Delay(1);
		temp>>=1;
		Sclk = 1;
		Delay(1);
	}
	Ce = 0;
}

uint8_t Read1302(uint8_t addr)
{
	uint8_t  i,temp,dat1,dat2;
	Ce=0; //оƬ���ڸ�λ״̬
	Sclk=0; //����
	Ce = 1; //оƬ��ʼ����
	//���͵�ַ
	for ( i=8; i>0; i-- ) //ѭ��8����λ
	{
		Sclk = 0; //ʱ��������
		temp = addr;
		Dio = (bit)(temp&0x01); //ÿ�δ����λ
		addr >>= 1; //����һλ
		Sclk = 1;
	}
    for( i=8; i>0; i-- ) 
    {
        ACC_7=Dio;
        Sclk = 1;
        ACC>>=1;
        Sclk = 0;
    } 
	Ce=0; //��ֹоƬ����
 
	dat1=ACC;
	dat2=dat1/16; //���ݽ���ת��,��ȡ����λ����Ϊʮλ
	dat1=dat1%16; //��ȡ����λ����Ϊ��λ
	dat1=dat1+dat2*10; //�õ���ʮ������
	return (dat1); //����ʮ����ֵ
}



#if 0

void Init_Ds1302(void)
{
		Write1302 (Write_Protect,0X00);         
    Write1302 (Write_Seond,0X00);         
    Write1302 (Write_Minute,0x22);        
    Write1302 (Write_Hour,0x12);                   
    Write1302 (Write_Protect,0x80); 
}

#endif



void Update_DateAndTime(void)
{
	Seond=Read1302(Read_Seond);
	Minute=Read1302(Read_Minute);
	Hour=Read1302(Read_Hour);
	Day=Read1302(Read_Day);
	Month=Read1302(Read_Month);
	Year=Read1302(Read_Year);
}



//init ds1302
void Set1302(void)
{
#if 1
	unsigned char MinuteTemp,HourTemp,DayTemp,MonthTemp,YearTemp;
	unsigned char a,b;

	a = (Minute/10<<4)&0xF0;
	b = Minute%10;
	
	MinuteTemp = a|b;

	a = (Hour/10<<4)&0xF0;
	b = Hour%10;
	
	HourTemp = a|b;

	a = (Day/10<<4)&0xF0;
	b = Day%10;
	
	DayTemp = a|b;

	a = (Month/10<<4)&0xF0;
	b = Month%10;
	
	MonthTemp = a|b;

	a = (Year/10<<4)&0xF0;
	b = Year%10;

	YearTemp = a|b;
#endif		
	Write1302(Write_Protect, 0x00);
	Write1302(Write_Seond,0x00);
	Write1302(Write_Minute, MinuteTemp);
	Write1302(Write_Hour,HourTemp);
	Write1302(Write_Day, DayTemp);
	Write1302(Write_Month, MonthTemp);
	Write1302(Write_Year, YearTemp);
	Write1302(0x90, 0x01);
	Write1302(0xc0, 0xf0);
	Write1302(Write_Protect, 0x80);

}
