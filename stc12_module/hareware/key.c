
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
   P0M0 &= ~(1 << 7);	//S3 配置为输入

   P4M1 |= (1 << 6);
   P4M0 &= ~(1 << 6);	//S2 配置为输入

   P0M1 |= (1 << 6);
   P0M0 &= ~(1 << 6);	//PowerKey 配置为输入	
	
	 P0M1 &= ~(1 << 5);
   P0M0 |= (1 << 5);	//PowerDown 配置为输出	

	 P4SW |= (1 << 6);
}





/******************************************
*函数名    ：key_scan
*函数功能  ：按键扫描函数
*函数参数  ：无
*函数返回值：u8
*函数描述  ：	
							①检测管脚电平状态        //按键是否按下
							②延时5~10ms
							③再次检测管脚电平状态    //按键是否真的按下
							④执行按键要操作的事件    //赋键值
							⑤等待按键抬起
							返回值键值
						 按键按下返回值对应的键值

*不用该	
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
//		if(!K_S3)         //按键3按下
//		{
//			key_val = 3;              //赋值键值
//			key_flag = 0;             //标志位锁定	
//			backlight_time = 60000;
//			//SendData(3);
//		}
//		else if(!K_S2)   //按键2按下
//		{
//			key_val = 2;              //赋值键值
//			key_flag = 0;             //标志位锁定
//			backlight_time = 60000;
//			//SendData(2);
//		}
//		else if(!K_S1)    //按键1按下
//		{
//			key_val = 1;              //赋值键值
//			key_flag = 0;             //标志位锁定
//			backlight_time = 60000;
//			//SendData(1);
//		}
//		
//		if(backlight_time <= 0)
//		{
//			key_val = 1;              //赋值键值
//			key_flag = 0;             //标志位锁定
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
//	if(K_S3 && K_S2 && K_S1)  //全部按键抬起
//	{
//		key_flag = 1;            //标志位重新置1允许下次按键扫描
//	}

//	
//	KeyValue = key_val;
//	F_NewKey = True;
//}


/******************************************
*函数名    ：ReadIOKey
*函数功能  ：按键口状态值读取
*函数参数  ：无
*函数返回值：u8
*函数描述  ：	(改进按键反应迟钝问题)
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
	*函数名    ：key_driver
	*函数功能  ：按键驱动函数
	*函数参数  ：无
	*函数返回值：u8
	*函数描述  ：	(改进按键反应迟钝问题)
	******************************************/

	void key_driver(void)
	{
		uint8_t TmpKeyVal;
		
		F_NewKey = False;
		TmpKeyVal = ReadIOKey();
		
		switch(Key_Change)
		{
			case KEY_STAT0:			//消抖
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
						Key_Change = KEY_STAT0;			//无按键
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
				
			case KEY_STAT3:								//短按返回
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

			  