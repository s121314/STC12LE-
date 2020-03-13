
#include "key.h"
#include "usart.h"


enum{
	KEY_STAT0 = 0,
	KEY_STAT1 ,
	KEY_STAT2,
	KEY_STAT3,
	KEY_STAT4,
};

uint8_t Key_Change = 0;
uint8_t rKeyBuff;

uint8_t KeyValue = 0;
bit F_NewKey;
extern uint16_t backlight_time ;
extern uint16_t ks_X1ms;

//P07 ---> S3
//P46 ---> S2
//P05 ---> PowerKey
void Key_Init(void)
{
   P0M1 |= (1 << 7);
   P0M0 &= ~(1 << 7);	//S3 ����Ϊ����

   P4M1 |= (1 << 6);
   P4M0 &= ~(1 << 6);	//S2 ����Ϊ����

   P0M1 |= (1 << 6);
   P0M0 &= ~(1 << 6);	//PowerKey ����Ϊ����	
	
	 P0M1 &= ~(1 << 5);
   P0M0 |= (1 << 5);	//PowerDown ����Ϊ���	

	 P4SW |= (1 << 6);
}





/******************************************
*������    ��key_scan
*��������  ������ɨ�躯��
*��������  ����
*��������ֵ��u8
*��������  ��	
							�ټ��ܽŵ�ƽ״̬        //�����Ƿ���
							����ʱ5~10ms
							���ٴμ��ܽŵ�ƽ״̬    //�����Ƿ���İ���
							��ִ�а���Ҫ�������¼�    //����ֵ
							�ݵȴ�����̧��
							����ֵ��ֵ
						 �������·���ֵ��Ӧ�ļ�ֵ

*���ø�	
******************************************/
//void key_scan(void)
//{
//	uint8_t key_val = 0xff;
//	static uint8_t key_flag = 1;
//	static uint8_t key_press_time = 0;
//		
//	if((!K_S3 || !K_S2 ||!K_S1) && key_flag)
//	{
//		Delay(1);
//		if(!K_S3)         //����3����
//		{
//			key_val = 3;              //��ֵ��ֵ
//			key_flag = 0;             //��־λ����	
//			backlight_time = 60000;
//			//SendData(3);
//		}
//		else if(!K_S2)   //����2����
//		{
//			key_val = 2;              //��ֵ��ֵ
//			key_flag = 0;             //��־λ����
//			backlight_time = 60000;
//			//SendData(2);
//		}
//		else if(!K_S1)    //����1����
//		{
//			key_val = 1;              //��ֵ��ֵ
//			key_flag = 0;             //��־λ����
//			backlight_time = 60000;
//			//SendData(1);
//		}
//		
//		if(backlight_time <= 0)
//		{
//			key_val = 1;              //��ֵ��ֵ
//			key_flag = 0;             //��־λ����
//			backlight_time = 60000;
//		}
//	}
//	
//	
//	if(key_flag == 0)
//	{
//		if(!K_S1)
//			key_press_time++;		
//		if(key_press_time > 40)
//		{
//			key_press_time = 0;
//			KeyValue = 6;
//			return;
//		}
//	}
//	
//	if(K_S3 && K_S2 && K_S1)  //ȫ������̧��
//	{
//		key_flag = 1;            //��־λ������1�����´ΰ���ɨ��
//	}

//	
//	KeyValue = key_val;
//	F_NewKey = True;
//}


/******************************************
*������    ��ReadIOKey
*��������  ��������״ֵ̬��ȡ
*��������  ����
*��������ֵ��u8
*��������  ��	(�Ľ�������Ӧ�ٶ�����)
******************************************/
uint8_t ReadIOKey(void)
{
	if(!K_S1)
		return (KEY_1);
	if(!K_S2)
		return (KEY_2);
	if(!K_S3)
		return (KEY_3);
	return (Key_None);
}



	/******************************************
	*������    ��key_driver
	*��������  ��������������
	*��������  ����
	*��������ֵ��u8
	*��������  ��	(�Ľ�������Ӧ�ٶ�����)
	******************************************/

	void key_driver(void)
	{
		uint8_t TmpKeyVal;
		
		F_NewKey = False;
		TmpKeyVal = ReadIOKey();
		
		switch(Key_Change)
		{
			case KEY_STAT0:			//����
			{
					if(TmpKeyVal != Key_None)
					{
						rKeyBuff = TmpKeyVal;
						Key_Change = KEY_STAT2;
						ks_X1ms = 3000;
					}
					return;
				}
			//	break;
			case KEY_STAT1:
			{
					if(TmpKeyVal != rKeyBuff)
					{
						Key_Change = KEY_STAT0;			//�ް���
					}
					else if(ks_X1ms == 0)
					{
						Key_Change = KEY_STAT2;
						ks_X1ms = 20;
					}
					return;
			}
			case KEY_STAT2:
				if(TmpKeyVal != rKeyBuff)
				{
					Key_Change = KEY_STAT3;
				}
				else if(ks_X1ms == 0)
				{
					 TmpKeyVal = rKeyBuff + KEY_TOTAL;
						Key_Change = KEY_STAT4;
						backlight_time = 60000;
						break;
				}
				return;
				
			case KEY_STAT3:								//�̰�����
					TmpKeyVal = rKeyBuff;
					Key_Change = KEY_STAT0;	
					backlight_time = 60000;
				break;
			
			case KEY_STAT4:
				if(TmpKeyVal != rKeyBuff)
				{
					Key_Change = KEY_STAT0;
				}
				return;
			
			default:
				break;
		}
		
		KeyValue = TmpKeyVal;
		F_NewKey = True;
	}

			  