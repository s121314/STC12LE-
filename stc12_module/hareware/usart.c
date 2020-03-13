
#include "usart.h"



void Usart_Init(uint16_t bound)
{
  	SM0 = 0;
	SM1 = 1;							//WORK FUNTION 1  (8 bit UART)

	TMOD |= 0x20; 						//timer1 work_group 1
	TH1 = TL1 = -(FOSC/12/32/bound); 	//setup auto_reload
	TR1 = 1;							//Timer1 run
		
}


/*----------------------------
Send one byte data to PC
Input: dat (UART data)
Output:-
----------------------------*/
void SendData(uint8_t dat)
{
//	while(!TI);
//	TI = 0;
	ACC = dat;
	
	SBUF = ACC;
}

//void SendData(uint8_t dat)
//{
//   while (!TI);                    //Wait for the previous data is sent
//   TI = 0;                         //Clear TI flag
//    SBUF = dat;                     //Send current data
//}


void SendTwo(uint16_t value)
{
	SendData((uint8_t)(value >> 8));
	Delay(100);
	SendData((uint8_t)value);
} 


