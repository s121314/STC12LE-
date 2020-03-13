#include "reg51.h"
#include "stc12.h"
#include "usart.h"
#include "ds1302.h"
#include "key.h"
#include "sound.h"
#include "intrins.h"
#include "adc.h"
#include "other.h"
#include "test.h"
#include "eeprom.h"

#include <stdio.h>
#include <string.h>



float vcc = 3.3;





//adc采集转化部分


void adc_buffer_init(void)
{
	memset(fire_buffer, 0, sizeof(fire_buffer));
	memset(hydrothion_buffer, 0, sizeof(hydrothion_buffer));
	memset(o2_buffer, 0, sizeof(o2_buffer));
	memset(co_buffer, 0, sizeof(co_buffer));
}




void Transform_AdcGas(void)
{
//	float fire_voltage;
//  float hydrothion_voltage;
//	float o2_voltage;
//	float co_voltage;
	uint8_t i;
	uint8_t sp_buffer[Record_Argurment_Num] = {0};

	adc_buffer_init();	//清buffer
	
	collect_adc_value();	//采集ADC 值
	
	
	/*******************************故障检测***********************************/
	if(hydrothion_gas_adc > FaultValueH || hydrothion_gas_adc < FaultValueL)
	{
		falue_flag |= 1 << 0;
	}
	if(o2_gas_adc > FaultValueH || o2_gas_adc < FaultValueL)
	{
		falue_flag |= 1 << 1;
	}
	if(co_gas_adc > FaultValueH || co_gas_adc < FaultValueL)
	{
		falue_flag |= 1 << 2;
	}
	if(fire_gas_adc > FaultValueH || fire_gas_adc < FaultValueL)
	{
		falue_flag |= 1 << 3;
	}


	/*******************************把ADC值转化为电压值***********************************/

//	fire_voltage = (vcc*fire_gas_adc)/1024;
//	hydrothion_voltage = (vcc*hydrothion_gas_adc)/1024;
//	o2_voltage = (vcc*o2_gas_adc)/1024;
//	co_voltage = (vcc*co_gas_adc)/1024;
	
	
//	sprintf(fire_buffer,"%.1f", fire_voltage);
//	sprintf(hydrothion_buffer,"%.1f", hydrothion_voltage);
//	sprintf(o2_buffer,"%.1f", o2_voltage);
//	sprintf(co_buffer,"%.1f", co_voltage);
	
//		sprintf(fire_buffer,"%d", fire_gas_adc);
		sprintf(hydrothion_buffer,"%d", hydrothion_gas_adc);
		sprintf(o2_buffer,"%d", o2_gas_adc);
//		sprintf(co_buffer,"%d", co_gas_adc);
	
	
	/********************在把电压值根据传感器的特性曲线转化为浓度值**************************/	
	//直接把相应的ADC值转化为浓度值，
	//转化公式是从试验数据得到的，通过EXCEL获得		
	
	//可燃气体的公式：y = 0.1925x + 1.2803 (x = adc-零点ADC)
		if(fire_gas_adc <= (original_fire_adc + 2))
		{
					fire_temp = 0;
		}
		else
		{
					fire_temp = (uint8_t)((fire_gas_adc - original_fire_adc)*0.1925 + 1.2803);
					if(fire_temp >= 100)
					 fire_temp = 100;	
		}
	
		//一氧化碳气体的公式：y = 1.5297x - 2.3535 (x = adc-零点ADC)
		if(co_gas_adc < 2)
		{
			co_temp = 0;
		}
		else
		{
			co_temp = (uint16_t)(co_gas_adc*1.5297 - 2.3535);
			if(co_temp >= 1000)
				co_temp = 1000;	
		}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	/********************对当前浓度的状态进行判断********************************************/	
	//硫化氢
//	if(hydrothion_temp >= hydrothion_LAlarm)
//	{
//		if(hydrothion_temp >= hydrothion_HAlarm)
//		{
//			High_alarm_flag |= (1 << 0);
//		}
//		Low_alarm_flag |= (1 << 0);
//	}
//	else
//	{
//		Low_alarm_flag &= ~(1 << 0);
//		High_alarm_flag &= ~(1 << 0);
//	}
	
//	//氧气
//	if(o2_temp <= o2_LAlarm.x)
//	{
//		Low_alarm_flag |= (1 << 1);
//	}
//	else if(o2_temp >= o2_HAlarm.x)
//	{
//		High_alarm_flag |= (1 << 1);
//	}
//	else
//	{
//		Low_alarm_flag &= ~(1 << 1);
//		High_alarm_flag &= ~(1 << 1);
//	}
	
	//一氧化碳
	if(co_temp >= co_LAlarm)
	{
		if(co_temp >= co_HAlarm)
		{
			High_alarm_flag |= (1 << 2);
		}
		Low_alarm_flag |= (1 << 2);
		if(max_co < co_temp)
		{
			max_co = co_temp;
		}
	}
	else
	{
			//在这个if 语句中完成一次记录保存
		if(max_co != 0)
		{
			if(max_co > co_HAlarm)
			{
				record_statue |= (1 << 2);
			}
			else
			{
				record_statue &= ~(1 << 2);
			}
			
			
			IapProgramByte(co_sp + 0 ,record_statue);	//第一个字节存气体类型和报警状态 
			Delay(15);
			IapProgramByte(co_sp + 1 ,max_co);
			Delay(15);
			IapProgramByte(co_sp + 2 ,Year);
			Delay(15);
			IapProgramByte(co_sp + 3 ,Month);
			Delay(15);
			IapProgramByte(co_sp + 4 ,Day);
			Delay(15);
			IapProgramByte(co_sp + 5 ,Hour);
			Delay(15);
			IapProgramByte(co_sp + 6 ,Minute);
			Delay(15);
			IapProgramByte(co_sp + 7 ,Seond);
			Delay(15);
			
			//在把fire_sp 存起来
			co_sp += 8;
			co_record_num += 1;	//数量自增
			
			if((co_sp + 8 ) > ADD_H2S_BASE)
			{
				co_sp = ADD_C0_BASE;
				IapEraseSector(ADD_C0_BASE);
				co_record_num -= 64;											//每个扇区	可以保存64条记录
			}
			else if(co_sp + 8 > 0xE00)									//第8扇区
			{
				IapEraseSector(0xE00);	
				if(co_record_num >256)
					co_record_num -= 64;						
			}
			else if(co_sp + 8 > 0x1000)									//第9扇区
			{
				IapEraseSector(0x1000);
				if(co_record_num >256)				
					co_record_num -= 64;		
			}
			else if(co_sp + 8 > 0x1200)									//第10扇区
			{
				IapEraseSector(0x1200);	
				if(co_record_num >256)
					co_record_num -= 64;		
			}
			else if(co_sp + 8 > 0x1400) 									//第11扇区
			{
				IapEraseSector(0x1400);
				if(co_record_num >256)				
					co_record_num -= 64;		
			}
			
		
			//co_sp 和 co_record_num
			for(i = 0; i < Record_Argurment_Num; i++)
			{
				sp_buffer[i] = IapReadByte(ADD_FIRE_SP_BASE + i);
				Delay(10);
			}
			
			sp_buffer[ADD_C0_NUM] = (uint8_t)(co_sp >> 8);
			sp_buffer[ADD_C0_NUM + 1] = (uint8_t) co_sp;
			sp_buffer[ADD_C0_COUNT] = (uint8_t)(co_record_num >> 8);
			sp_buffer[ADD_C0_COUNT + 1] = (uint8_t)co_record_num;
			
			IapEraseSector(ADD_FIRE_SP_BASE);
			for(i = 0; i < Record_Argurment_Num; i++)
			{
				IapProgramByte(ADD_FIRE_SP_BASE + i, sp_buffer[i]);
				Delay(10);
			}
			max_co = 0;
		}
		
		
		Low_alarm_flag &= ~(1 << 2);
		High_alarm_flag &= ~(1 << 2);
	}
	
	//可燃气体
	if(fire_temp >=  fire_LAlarm)
	{
		if(fire_temp >= fire_HAlarm)
			High_alarm_flag |= (1 << 3);
		Low_alarm_flag |= (1 << 3);
		
		if(max_fire < fire_temp)
		{
			max_fire = fire_temp;
		}
	}
	else
	{
		//在这个if 语句中完成一次记录保存
		if(max_fire != 0)
		{
			if(max_fire > fire_HAlarm)
			{
				record_statue |= (1 << 3);
			}
			else
			{
				record_statue &= ~(1 << 3);
			}
			
			
			IapProgramByte(fire_sp + 0 ,record_statue);	//第一个字节存气体类型和报警状态 
			Delay(15);
			IapProgramByte(fire_sp + 1 ,max_fire);
			Delay(15);
			IapProgramByte(fire_sp + 2 ,Year);
			Delay(15);
			IapProgramByte(fire_sp + 3 ,Month);
			Delay(15);
			IapProgramByte(fire_sp + 4 ,Day);
			Delay(15);
			IapProgramByte(fire_sp + 5 ,Hour);
			Delay(15);
			IapProgramByte(fire_sp + 6 ,Minute);
			Delay(15);
			IapProgramByte(fire_sp + 7 ,Seond);
			Delay(15);
			
			//在把fire_sp 存起来
			fire_sp += 8;
			fire_record_num += 1;	//数量自增
			
			if((fire_sp + 8 ) > ADD_C0_BASE)
			{
				fire_sp = ADD_FIRE_BASE;
				IapEraseSector(ADD_FIRE_BASE);
				fire_record_num -= 64;											//每个扇区	可以保存64条记录
			}
			else if(fire_sp + 8 > 0x400)									//第3扇区
			{
				IapEraseSector(0x400);	
				if(fire_record_num >256)
					fire_record_num -= 64;						
			}
			else if(fire_sp + 8 > 0x600)									//第4扇区
			{
				IapEraseSector(0x600);
				if(fire_record_num >256)				
					fire_record_num -= 64;		
			}
			else if(fire_sp + 8 > 0x800)									//第5扇区
			{
				IapEraseSector(0x800);	
				if(fire_record_num >256)
					fire_record_num -= 64;		
			}
			else if(fire_sp + 8 > 0xA00) 									//第6扇区
			{
				IapEraseSector(0xA00);
				if(fire_record_num >256)				
					fire_record_num -= 64;		
			}
			
		
			//存储fire_sp 和 fire_record_num
			for(i = 0; i < Record_Argurment_Num; i++)
			{
				sp_buffer[i] = IapReadByte(ADD_FIRE_SP_BASE + i);
				Delay(10);
			}
			
			sp_buffer[ADD_FIRE_NUM] = (uint8_t)(fire_sp >> 8);
			sp_buffer[ADD_FIRE_NUM + 1] = (uint8_t) fire_sp;
			sp_buffer[ADD_FIRE_COUNT] = (uint8_t)(fire_record_num >> 8);
			sp_buffer[ADD_FIRE_COUNT + 1] = (uint8_t)fire_record_num;
			
			IapEraseSector(ADD_FIRE_SP_BASE);
			for(i = 0; i < Record_Argurment_Num; i++)
			{
				IapProgramByte(ADD_FIRE_SP_BASE + i, sp_buffer[i]);
				Delay(10);
			}
			max_fire = 0;
		}
		Low_alarm_flag &= ~(1 << 3);
		High_alarm_flag &= ~(1 << 3);
	}
}





void monitor_interface(void)
{
	if(language == Chinese)
	{
		lcd_show_string(0,0,"硫化氢");
		lcd_show_string(80,0,"氧气");
		lcd_show_string(0,4,"一氧化碳");
		lcd_show_string(80,4,"可燃");
	}
	else if(language == English)
	{
		lcd_draw_string(0, 0, "H2S");
		lcd_draw_string(80, 0, "O2");
		lcd_draw_string(0, 4, "CO");
		lcd_draw_string(80, 4, "EX");
	}
	
	//可燃气体部分
	glcd_clear(6,72,7,96);
	if(fire_temp >= 100)
	{
		fire_buffer[0] = '0'+1;
		fire_buffer[1] = '0'+ 0;
		fire_buffer[2] = '0'+ 0;
		fire_buffer[3] = '\0';
		lcd_draw_string(72,6,fire_buffer);
	}
	else if(fire_temp < 10)
	{
		fire_buffer[0] = '0'+ fire_temp;
		lcd_draw_string(88,6,fire_buffer);
	}
	else
	{
		fire_buffer[0] = '0'+ fire_temp/10;
		fire_buffer[1] = '0'+ fire_temp%10;
		fire_buffer[2] = '\0'; 
		lcd_draw_string(80,6,fire_buffer);
	}
	lcd_draw_string(96,6,"%LEL");
	
	
	//co 部分
	glcd_clear(6,8,7,40);
	if(co_temp >= 1000)
	{
		co_buffer[0] = '0' + 1; 
		co_buffer[1] = '0' + 0; 
		co_buffer[2] = '0' + 0;
		co_buffer[3] = '0' + 0;
		co_buffer[4] = '\0'; 
		lcd_draw_string(8,6,co_buffer);
	}
	else if(co_temp >= 100)
	{
		co_buffer[0] = '0' + co_temp/100; 
		co_buffer[1] = '0' + co_temp%100/10; 
		co_buffer[2] = '0' + co_temp%10;
		co_buffer[3] = '\0' ; 
		lcd_draw_string(16,6,co_buffer);
	}
	else if(co_temp >= 10)
	{
		co_buffer[0] = '0' + co_temp/10; 
		co_buffer[1] = '0' + co_temp%10; 
		co_buffer[2] = '\0' ; 
		lcd_draw_string(24,6,co_buffer);
	}
	else 
	{
		co_buffer[0] = '0' + co_temp%10; 
		co_buffer[1] = '\0' ; 
		lcd_draw_string(32,6,co_buffer);
	}
	lcd_draw_string(40, 6, "PPM");
	
	
	lcd_draw_string(32,2,hydrothion_buffer);
	lcd_draw_string(80,2,o2_buffer);
}

void circulation_test(void)
{
	static uint8_t i = 0;
	
	reset();
	send(i);
	while(busy);
	i++;
	if(i >= 128)
		i = 0;
}




/******************************************
*	函数说明：声音报警处理函数
*
********************************************/

void Handle_Sound(void)
{

	switch(sound_chose)
	{
		case 0:
			
			reset();
			if((Low_alarm_flag & (1 << 3)) && !(Handle_sound_flag & (1 << 3)))
			{
				send(sound_fire_alarm);
				Handle_sound_flag |= (1 << 3);
			}
			else if((Low_alarm_flag & (1 << 2)) && !(Handle_sound_flag & (1 << 2)))
			{
				send(sound_co_alarm);
				Handle_sound_flag |= (1 << 2);
			}
			
//			else if((High_alarm_flag & (1 << 1)) && !(Handle_sound_flag & (1 << 1)))
//			{
//				send(sound_o2_alarm);
//				Handle_sound_flag |= (1 << 1);
//			}
//			else if((Low_alarm_flag & (1 << 0)) && !(Handle_sound_flag & (1 << 0))
//			{
//				send(sound_hydrothion_alarm);
//				Handle_sound_flag |= (1 << 0);
//			}
			
			
		//	while(busy);
		
			sound_chose = 1;
		 break;
		
		
		case 1:
			reset();
			send(sound_ndcb);
			sound_chose = 2;
		 break;
		
		
		case 2:
		 reset();
		 send(sound_charge1_low);
			
		if(((Low_alarm_flag & (1 << 3)) && !(Handle_sound_flag & (1 << 3))) || ((Low_alarm_flag & (1 << 2)) && !(Handle_sound_flag & (1 << 2))))
			sound_chose = 0;
		
			
		break;
	}
}






/*******************************************
*函数功能: 把灯的闪烁和震动组合在一起
*	
*	传入的参数: uint16_t pss (闪烁的频率)
*
*
*	遇到的问题：声音播报会影响到 灯闪烁和震动的频率，
*		初步判断是：声音发送时间过长，影响到了灯的频率
*		解决办法：把声音处理中的while(busy);这个判断屏蔽
********************************************/

void alarm_shake(uint16_t pss)
{
	if(shake_num <= 0)
	{
		shake_num = pss;
		LED_1 = ~LED_1;
		if(fflag == 0)
		{
			Shake_Pin(1);
			fflag = 1;
		}
		else 
		{
			Shake_Pin(0);
			fflag = 0;
		}
	}
	if(sound_delaay_num <= 0)
	{
			sound_delaay_num = 1800;
			Handle_Sound();
	}
}



/*******************************************
*函数功能: 对传感器的状态进行判断，并做出对应的动作
*	
*	传入的参数: 无
*						
********************************************/

void Do_Alarm(void) 
{
	if(Low_alarm_flag != 0)
	{	
		alarm_shake(250);
	}
	else 
	{
		Shake_Pin(0);
		LED_1 = 0;
		sound_chose = 0;
	}
}














//时间采集部分

void lcd_show_time(void)
{
	uint8_t temp[5];
	
	temp[0] = '0' + Year/10;
	temp[1] = '0' + Year%10;
	temp[2] = '\0';

	lcd_draw_string(64,4, temp);
	lcd_draw_char(80, 4,'.');

	temp[0] = '0' + Month/10;
	temp[1] = '0' + Month%10;
	temp[2] = '\0';
	
	lcd_draw_string(88,4, temp);
	lcd_draw_char(104, 4,'.');

	temp[0] = '0'+Day/10;
	temp[1] = '0'+Day%10;
	temp[2] = '\0';
	
	lcd_draw_string(112,4, temp);


	temp[0] = '0'+Hour/10;
	temp[1] = '0'+Hour%10;
	temp[2] = '\0';
	lcd_draw_string(64,6, temp);
	lcd_draw_char(80,6,':');

	temp[0] = '0'+Minute/10;
	temp[1] = '0'+Minute%10;
	temp[2] = '\0';
 	lcd_draw_string(88,6, temp);
	lcd_draw_char(104, 6,':');

	temp[0] = '0'+Seond/10;
	temp[1] = '0'+Seond%10;
	temp[2] = '\0';
	lcd_draw_string(112,6, temp);
}



#if 0
void lcd_show_time(void)
{
	uint8_t date_buffer[15] = {0};
	
	sprintf(date_buffer, "%02d.%02d.%02d", Year,Month,Day);
	lcd_draw_string(64,4,date_buffer);
	memset(date_buffer, 0, sizeof(date_buffer));
	sprintf(date_buffer, "%02d:%02d:%02d", Hour,Minute,Seond);
	lcd_draw_string(64,6,date_buffer);
}
#endif






void information_interface(void)
{
	uint8_t day_temp = 0;
	uint8_t minute_temp = 0;
	uint8_t hour_temp = 0;
	uint8_t month_temp = 0;
	uint8_t m_temp[9] = {0};
	
	
	day_temp = Day - original_day;
	minute_temp = Minute - original_minute;
	hour_temp = Hour - original_houre;
	month_temp = Month - 	original_month;
	
	sum_minute = month_temp*30*24*60 + day_temp*24*60 + hour_temp*60 + minute_temp;
	//sum_minute = 10;
	if(language == Chinese)
	{
		lcd_show_string(0,0,"电量");
		lcd_show_string(0,2,"已监测");
		lcd_show_string(0,4,"日期");
		lcd_show_string(0,6,"时间");
	}
	else if(language == English)
	{
		lcd_draw_string(0,0,"BAT");
		lcd_draw_string(0,2,"Testing");
		lcd_draw_string(0,4,"Date");
		lcd_draw_string(0,6,"Time");
	}
	if(sum_minute >= 9999)
		sum_minute = 9999;
	
	if(sum_minute >= 1000)
	{
		m_temp[0] = '0' + sum_minute /1000;
		m_temp[1] = '0' + sum_minute %1000 /100;
		m_temp[2] = '0' + sum_minute % 100/10;
		m_temp[3] = '0' + sum_minute %10;
		m_temp[4] = '\0';
		lcd_draw_string(72,2,m_temp);		
	}
	else if(sum_minute >= 100)
	{
		m_temp[0] = '0' + sum_minute /100;
		m_temp[1] = '0' + sum_minute %100 /10;
		m_temp[2] = '0' + sum_minute % 10;
		m_temp[3] = '\0';
		lcd_draw_string(80,2,m_temp);
	}
	else if(sum_minute >= 10)
	{
		m_temp[0] = '0' + sum_minute /10;
		m_temp[1] = '0' + sum_minute %10;
		m_temp[2] = '\0';
		lcd_draw_string(88,2,m_temp);
	}
	else
	{
		lcd_draw_char(96,2,'0' + sum_minute);
	}
	

	lcd_draw_string(104,2,"min");
	lcd_draw_battery(80,0);
	
	
	lcd_show_time();
	
//		lcd_show_string(0,0,"日期");
//		lcd_show_string(0,4,"验证");
//		lcd_show_string(0,6,"时间");
//		lcd_draw_string(88,2,"73");
}

void passwork_interface(void)
{
	if(language == Chinese)
	{
		lcd_show_string(32,0,"密码验证");
		lcd_show_string(0,6,"增加");
		lcd_show_string(48,6,"确认");
		lcd_show_string(96,6,"减少");
	}
	else if(language == English)
	{
		lcd_draw_string(32,0,"Password");
		lcd_draw_string(0,6,"Up");
		lcd_draw_string(40,6,"Enter");
		lcd_draw_string(96,6,"Down");
	}
	

//	lcd_draw_char(32,3,'0' + PassTemp[0]);
//	lcd_draw_char(48,3,'0' + PassTemp[1]);
//	lcd_draw_char(64,3,'0' + PassTemp[2]);
//	lcd_draw_char(80,3,'0' + PassTemp[3]);
	
}


void menu_interface(void)
{
	static uint8_t pages = 0;
	static uint8_t points = 0;
	
	if(SubMenuWorkSt != SUB_MENU_IDLE)
	{
			 Handle_SubMenu();
			 return;
	}
	
	if(language == Chinese)
	{
		lcd_show_string(48,0,"菜单");
	}
	else if(language == English)
	{
		lcd_draw_string(48,0,"Menu");
	}
	
	
	lcd_draw_char(8,2+points*2,'>');
	

	
	switch(pages)
	{
		case 0:
				//第一页
			if(language == Chinese)
			{
				lcd_show_string(16,2,"报警记录");
				lcd_show_string(16,4,"低报设置");
				lcd_show_string(16,6,"高报设置");
			}
			else if(language == English)
			{
				lcd_draw_string(16,2,"Alarm Record");
				lcd_draw_string(16,4,"Set L Alarm");
				lcd_draw_string(16,6,"Set H Alarm");
			}

		 break;
		
		case 1:
				//第二页
			if(language == Chinese)
			{
				lcd_show_string(16,2,"零点设置");
				lcd_show_string(16,4,"标定设置");
				lcd_show_string(16,6,"时间设置");	
			}
			else if(language == English)
			{
				lcd_draw_string(16,2,"Zero Adjust");
				lcd_draw_string(16,4,"Cali Gas");
				lcd_draw_string(16,6,"Set Time");	
			}
		
		 break;
		
		case 2:
				//第三页
			if(language == Chinese)
			{
				lcd_show_string(16,2,"背光设置");
				lcd_show_string(16,4,"密码设置");
				lcd_show_string(16,6,"通道信息");		
			}
			else if(language == English)
			{
				lcd_draw_string(16,2,"Set Backlight");
				lcd_draw_string(16,4,"Set Password");
				lcd_draw_string(16,6,"Channel info");	
			}
		
		 break;
		
		case 3:
				//第四页
			if(language == Chinese)
			{
				lcd_show_string(16,2,"语言切换");
				lcd_show_string(16,4,"退出");
			}
			else if(language == English)
			{
				lcd_draw_string(16,2,"Set Language");
				lcd_draw_string(16,4,"Exit");
			}
			
		 break;	
		
			
		default:
			break;
	}
	
	
		//按键处理
	if(KeyValue == 3 && F_NewKey == True)
	{
		F_NewKey = False;
		glcd_clear(2, 0, 7, 16);
		if(points > 0)
		{
			points--;		
		}
		else 
		{
			glcd_clear_now();
			if(pages > 0)
			{
				pages--;	
				points = 2;
			}
			else
			{
				pages = 3;
				points = 1 ;
			}
		}
	}
	else if(KeyValue == 2 && F_NewKey == True)
	{
		F_NewKey = False;
		glcd_clear(2, 0, 7, 16);
		if((pages != 3 && points < 2)||((pages == 3) && points < 1))
		{
			points++;
		}
		else
		{
			glcd_clear_now();
			points = 0;
			if(pages < 3)
			{
				pages++;
			}
			else
				pages = 0;
		}
	}
	else if(KeyValue == 1 && F_NewKey == True)
	{
		F_NewKey = False;
		TS_Story = backlight_arg;	//背光比较特殊点
		La_Story = language;
		
		sub_point = 0;
		SubMenuWorkSt = pages*3 + points + 1;
		glcd_clear_now();
		memset(PassTemp, 0, sizeof(PassTemp));
		PassFlag = 0;
		pages = 0;
		points = 0;
	}
}




void Handle_SubMenu(void)		//子菜单处理入口
{
	static uint8_t data_buffer[60] = {0};	//暂存系统数据
	static uint8_t change_flag = 0;
	uint8_t i = 0;
	uint8_t temp[Record_Argurment_Num] = {0};
	static uint8_t sub_temp;
	static float sub_O2_temp;
	uint8_t *p_temp;
	static uint8_t record_point = 0;
	uint8_t tt_buffer[8] = {0};
	static uint16_t record_temp = 0;
	
	
	
	switch(SubMenuWorkSt)
	{
		case SUB_MENU_IDLE:
			break;
		
		case SUB_MENU_RECORD:
			if(language == Chinese)
				{
					lcd_show_string(32, 0, "通道选择");
					lcd_show_string(0, 6, "向上");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "向下");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Select Gas");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

		
				switch(sub_point)
				{
					case 0:
							if(language == Chinese)
								lcd_show_string(40, 3, "硫化氢");
							else if(language == English)
								lcd_draw_string(40, 3, "H2S");
							
						break;
					case 1:
							if(language == Chinese)
								lcd_show_string(48, 3, "氧气");	
							else if(language == English)
								lcd_draw_string(48, 3, "O2");
						break;
					case 2:
							if(language == Chinese)
								lcd_show_string(32, 3, "一氧化碳");
							else if(language == English)
								lcd_draw_string(48, 3, "CO");
						break;
					case 3:
							if(language == Chinese)
								lcd_show_string(48, 3, "可燃");
							else if(language == English)
								lcd_draw_string(48, 3, "EX");
						break;
				}
				
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point > 0)
					{
						sub_point--;
					}
					else
					{
						sub_point=3;
					}
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point < 3)
					{
						sub_point++;
					}
					else
					{
						sub_point = 0;
					}
				}
				else if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear_now();
					
					switch(sub_point)
					{
						case 0:
								SubMenuWorkSt = SUB_H2S_RECORD;
								record_temp = h2s_record_num;
								sp_record_temp = h2s_sp;
								
								pop_up_time = 2000;
							break;
						case 1:
								SubMenuWorkSt = SUB_02_RECORD;
								record_temp = o2_record_num;
								sp_record_temp = o2_sp;
							
								pop_up_time = 2000;
							break;
						case 2:
								SubMenuWorkSt = SUB_CO_RECORD;
								record_temp = co_record_num;
								sp_record_temp = co_sp;
								pop_up_time = 2000;
							break;
						case 3:
								SubMenuWorkSt = SUB_FIRE_RECORD;
								record_temp = fire_record_num;
								sp_record_temp = fire_sp;
								pop_up_time = 2000;
							break;
						
						default:
							break;
					}
				
				}
					
			break;		
		
		
		
////////////////////////////////////////////////////////////////////////		
		case SUB_MENU_LOWALARM:
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "通道选择");
					lcd_show_string(0, 6, "向上");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "向下");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Select Gas");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

		
				switch(sub_point)
				{
					case 0:
							if(language == Chinese)
								lcd_show_string(40, 3, "硫化氢");
							else if(language == English)
								lcd_draw_string(40, 3, "H2S");
							
						break;
					case 1:
							if(language == Chinese)
								lcd_show_string(48, 3, "氧气");	
							else if(language == English)
								lcd_draw_string(48, 3, "O2");
						break;
					case 2:
							if(language == Chinese)
								lcd_show_string(32, 3, "一氧化碳");
							else if(language == English)
								lcd_draw_string(48, 3, "CO");
						break;
					case 3:
							if(language == Chinese)
								lcd_show_string(48, 3, "可燃");
							else if(language == English)
								lcd_draw_string(48, 3, "EX");
						break;
				}
				
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point > 0)
					{
						sub_point--;
					}
					else
					{
						sub_point=3;
					}
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point < 3)
					{
						sub_point++;
					}
					else
					{
						sub_point = 0;
					}
				}
				else if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear_now();
					switch(sub_point)
					{
						case 0:
								SubMenuWorkSt = SUB_LOWALARM_HySet;
								change_flag = hydrothion_LAlarm_flag;
								sub_temp = hydrothion_LAlarm;
							break;
						case 1:
								SubMenuWorkSt = SUB_LOWALARM_O2Set;
								change_flag = o2_LAlarm_flag;
								sub_O2_temp = o2_LAlarm.x;
							break;
						case 2:
								SubMenuWorkSt = SUB_LOWALARM_CoSet;
								change_flag = co_LAlarm_flag;
								sub_temp = co_LAlarm;
							break;
						case 3:
								SubMenuWorkSt = SUB_LOWALARM_FireSet;
								change_flag = fire_LAlarm_flag;
								sub_temp = fire_LAlarm;
							break;
						default:
							break;
					}
				}
					
			break;
			
		case SUB_MENU_HIGHTALARM:
			
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "通道选择");
					lcd_show_string(0, 6, "向上");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "向下");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Select Gas");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

		
				switch(sub_point)
				{
					case 0:
							if(language == Chinese)
								lcd_show_string(40, 3, "硫化氢");
							else if(language == English)
								lcd_draw_string(40, 3, "H2S");
						break;
					case 1:
							if(language == Chinese)
								lcd_show_string(48, 3, "氧气");
							else if(language == English)
								lcd_draw_string(48, 3, "O2");
						break;
					case 2:
							if(language == Chinese)
								lcd_show_string(32, 3, "一氧化碳");
							else if(language == English)
								lcd_draw_string(32, 3, "CO");
						break;
					case 3:
							if(language == Chinese)
								lcd_show_string(48, 3, "可燃");
							else if(language == English)
								lcd_draw_string(48, 3, "EX");
						break;
				}
				
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point > 0)
					{
						sub_point--;
					}
					else
					{
						sub_point=3;
					}
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point < 3)
					{
						sub_point++;
					}
					else
					{
						sub_point = 0;
					}
				}
				else if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear_now();
					switch(sub_point)
					{
						case 0:
								SubMenuWorkSt = SUB_HIGHTALARM_HySet;
								change_flag = hydrothion_HAlarm_flag;
								sub_temp = hydrothion_HAlarm;
							break;
						case 1:
								SubMenuWorkSt = SUB_HIGHTALARM_O2Set;
								change_flag = o2_HAlarm_flag;
								sub_O2_temp = o2_HAlarm.x;
							break;
						case 2:
								SubMenuWorkSt = SUB_HIGHTALARM_CoSet;
								change_flag = co_HAlarm_flag;
								sub_temp = co_HAlarm;
							break;
						case 3:
								SubMenuWorkSt = SUB_HIGHTALARM_FireSet;
								change_flag = fire_HAlarm_flag;
								sub_temp = fire_HAlarm;
							break;
						default:
							break;
					}
				}
					
			break;				
				
				
		case SUB_MENU_CALIBRATION:
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "通道选择");
					lcd_show_string(0, 6, "向上");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "向下");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Select Gas");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

		
				switch(sub_point)
				{
					case 0:
							if(language == Chinese)
								lcd_show_string(40, 3, "硫化氢");
							else if(language == English)
								lcd_draw_string(40, 3, "H2S");
						break;
					case 1:
							if(language == Chinese)
								lcd_show_string(48, 3, "氧气");
							else if(language == English)
								lcd_draw_string(48, 3, "O2");
						break;
					case 2:
							if(language == Chinese)
								lcd_show_string(32, 3, "一氧化碳");
							else if(language == English)
								lcd_draw_string(48, 3, "CO");
						break;
					case 3:
							if(language == Chinese)
								lcd_show_string(48, 3, "可燃");
							else if(language == English)
								lcd_draw_string(48, 3, "EX");
						break;
				}
				
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point > 0)
					{
						sub_point--;
					}
					else
					{
						sub_point=3;
					}
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear(3, 32, 4, 98);
					if(sub_point < 3)
					{
						sub_point++;
					}
					else
					{
						sub_point = 0;
					}
				}
				else if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					change_flag = Nothing;
					glcd_clear_now();
					switch(sub_point)
					{
						case 0:
								SubMenuWorkSt = SUB_ZERO_HySet;
							break;
						case 1:
								SubMenuWorkSt = SUB_ZERO_O2Set;
							break;
						case 2:
								SubMenuWorkSt = SUB_ZERO_CoSet;
							break;
						case 3:
								SubMenuWorkSt = SUB_ZERO_FireSet;
							break;
						default:
							break;
					}
				}			
			break;
							

	 case SUB_MENU_SENSOR_ALARM_CNT:
		



		
		break;

				
				
					
		case SUB_MENU_TIME_SET:
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "时间设置");
					lcd_show_string(0, 6, "增加");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "减少");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Set Time");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

				
		
				switch(sub_point)
				{
					case 0:
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(16,2, temp);
						lcd_draw_char(32, 2,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(40,2, temp);
						lcd_draw_char(56, 2,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(64,2, temp);

						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');
							
						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_shadow_draw_string(112,4, temp);

						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							Seond++;
							if(Seond >= 61)
							{
								Seond = 0;
							}
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(Seond <= 0)
							{
								Seond = 60;
							}
							else
							{
								Seond--;
							}
						}		
						break;
						
					case 1:
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(16,2, temp);
						lcd_draw_char(32, 2,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(40,2, temp);
						lcd_draw_char(56, 2,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(64,2, temp);

						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_shadow_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');
							
						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);		
						
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							Minute++;
							if(Minute >= 61)
							{
								Minute = 0;
							}
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(Minute <= 0)
							{
								Minute = 60;
							}
							else
							{
								Minute--;
							}
						}		
						
						
					
						break;
						
					case 2:
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(16,2, temp);
						lcd_draw_char(32, 2,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(40,2, temp);
						lcd_draw_char(56, 2,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(64,2, temp);

						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_shadow_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');
							
						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);		
						
						
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							Hour++;
							if(Hour >= 23)
							{
								Hour = 0;
							}
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(Hour <= 0)
							{
								Hour = 23;
							}
							else
							{
								Hour--;
							}
						}					
						
						
					
						break;
						
					case 3:
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(16,2, temp);
						lcd_draw_char(32, 2,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(40,2, temp);
						lcd_draw_char(56, 2,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_shadow_draw_string(64,2, temp);

						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');
							
						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);	
											
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							Day++;
							if(Day >= 32)
							{
								Day = 0;
							}
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							if(Day <= 0)
							{
								Day = 31;
							}
							else
							{
								Day--;
							}
						}
					
						break;
						
					case 4:
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(16,2, temp);
						lcd_draw_char(32, 2,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_shadow_draw_string(40,2, temp);
						lcd_draw_char(56, 2,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(64,2, temp);

						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');
							
						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);
						
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							Month++;
							if(Month >= 13)
							{
								Month = 0;
							}
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							if(Month <= 0)
							{
								Month = 12;
							}
							else
							{
								Month--;
							}
						}


						break;
						
					case 5:
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_shadow_draw_string(16,2, temp);
						lcd_draw_char(32, 2,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(40,2, temp);
						lcd_draw_char(56, 2,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(64,2, temp);

						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');
							
						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);	
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							Year++;
							if(Year >= 100)
							{
								Year = 0;
							}
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(Year <= 0)
							{
								Year = 99;
							}
							else
							{
								Year--;
							}
						}
						break;	
						
					default:
						break;	
				}
				
			  if(KeyValue == 1 && F_NewKey == True)
				{
					glcd_clear_now();
					sub_point++;
					if(sub_point >= 6)
					{
						SubMenuWorkSt = SUB_SAVE_WINDOWN;
					}
				}			
			break;
				
				
		case SUB_MENU_BACKLIGHT:
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "背光设置");
					lcd_show_string(0, 6, "向上");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "向下");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Set Backlight");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}
	

				if(TS_Story == 0)
				{
					if(language == Chinese)
						lcd_show_string(48 ,3, "自动");
					else if(language == English)
						lcd_draw_string(48, 3, "auto");
				}
				else if(TS_Story == 1)
				{
					if(language == Chinese)
						lcd_show_string(48 ,3, "常亮");
					else if(language == English)
						lcd_draw_string(48 ,3, "Normally ON");
				}
				
				if((KeyValue == 3 || KeyValue == 2) && F_NewKey == True)
				{
					F_NewKey = False;
					TS_Story = TS_Story?0:1;
				}
				if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear_now();
					SubMenuWorkSt = SUB_SAVE_WINDOWN;
					change_flag = backlight_arg_flag;

					for(i = 0; i < Sector_Argurment_Num; i++)
					{
						data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
						Delay(2);
					}
					data_buffer[ADD_Backlight_Arg - 1] = TS_Story;
				}
			break;
				
				
		case SUB_MENU_PASSWORK:
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "密码验证");
					lcd_show_string(0, 6, "增加");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "减少");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Password");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

				

		
				switch(sub_point)
				{
					case 0:
						lcd_shadow_draw_char(32,3,'0' + PassTemp[0]);
						lcd_draw_char(48,3,'0' + PassTemp[1]);
						lcd_draw_char(64,3,'0' + PassTemp[2]);
						lcd_draw_char(80,3,'0' + PassTemp[3]);
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							PassTemp[0]++;
							if(PassTemp[0] >= 10)
							{
								PassTemp[0] = 0;
							}	
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(PassTemp[0] <= 0)
									PassTemp[0] = 9;
							else
									PassTemp[0]--;
						}
						break;
					
					case 1:
						lcd_draw_char(32,3,'0'+ PassTemp[0]);
						lcd_shadow_draw_char(48,3,'0'+ PassTemp[1]);
						lcd_draw_char(64,3,'0'+ PassTemp[2]);
						lcd_draw_char(80,3,'0'+ PassTemp[3]);
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							PassTemp[1]++;
							if(PassTemp[1] >= 10)
							{
								PassTemp[1] = 0;
							}	
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(PassTemp[1] <= 0)
									PassTemp[1] = 9;
							else
								PassTemp[1]--;
						}
					
					
						break;
					
					case 2:
					  lcd_draw_char(32,3,'0'+ PassTemp[0]);
						lcd_draw_char(48,3,'0'+ PassTemp[1]);
						lcd_shadow_draw_char(64,3,'0'+ PassTemp[2]);
						lcd_draw_char(80,3,'0'+ PassTemp[3]);
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							PassTemp[2]++;
							if(PassTemp[2] >= 10)
							{
								PassTemp[2] = 0;
							}	
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(PassTemp[2] <= 0)
									PassTemp[2] = 9;
							else
								PassTemp[2]--;
						}
						break;
					
					case 3:
						lcd_draw_char(32,3,'0'+ PassTemp[0]);
						lcd_draw_char(48,3,'0'+ PassTemp[1]);
						lcd_draw_char(64,3,'0'+ PassTemp[2]);
						lcd_shadow_draw_char(80,3,'0'+ PassTemp[3]);
						if(KeyValue == 3 && F_NewKey == True)
						{
							F_NewKey = False;
							PassTemp[3]++;
							if(PassTemp[3] >= 10)
							{
								PassTemp[3] = 0;
							}	
						}
						else if(KeyValue == 2 && F_NewKey == True)
						{
							F_NewKey = False;
							if(PassTemp[3] <= 0)
									PassTemp[3] = 9;
							else
								PassTemp[3]--;
						}					
						break;
					
					default:
						break;
				}
				if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					if(PassTemp[sub_point] != PassWord[sub_point])
					{
						PassFlag = 1;
					}
					sub_point++;
					if(sub_point >= 4)
					{
						if(PassFlag == 1)
						{
							glcd_clear_now();
							SubMenuWorkSt = SUB_MENU_IDLE;
							SystemWorkSt = NORMAL_WORK_PAGE;
						}
						else 
						{
							glcd_clear_now();
							sub_point = 0;
							SubMenuWorkSt = SUB_PASSWORK_CHANGE;
							memset(PassTemp, 0, sizeof(PassTemp));
						}
					}
				}
		
			break;
				
				
		case SUB_MENU_CHANEL:
			if(tnum <= 0)
			{
				memset(temp, 0, sizeof(temp));
				tnum = 3000;
				sub_point++;
				glcd_clear_now();
				if(sub_point >= 5)
				{
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				}
			}
			
			switch(sub_point)
			{
				case 1:
					
					if(language == Chinese)
					{
						lcd_show_string(40, 0, "硫化氢");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(40, 0, "H2S");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
					temp[0] = '0' + hydrothion_LAlarm / 10;
					temp[1] = '0' + hydrothion_LAlarm %10;
					temp[2] = '\0';
	
					lcd_draw_string(104, 2, "PPM");
					lcd_draw_string(88, 2, temp);
					
				  temp[0] = '0' + hydrothion_HAlarm / 10;
					temp[1] = '0' + hydrothion_HAlarm %10;
					temp[2] = '\0';				
					lcd_draw_string(104, 4, "PPM");
					lcd_draw_string(88, 4, temp);
				
				
					lcd_draw_string(80, 6, "100PPM");
					break;
					
				case 2:
					if(language == Chinese)
					{
						lcd_show_string(48, 0, "氧气");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(48, 0, "O2");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
				
					
				
					sprintf(temp, "%.1f", o2_LAlarm.x);
					lcd_draw_char(120, 2, '%');
					lcd_draw_string(88, 2, temp);
					
					sprintf(temp, "%.1f", o2_HAlarm.x);		
					lcd_draw_char(120, 4, '%');
					lcd_draw_string(88, 4, temp);
				
				
					
					lcd_draw_string(88, 6, "30.0%");
					break;
			
				case 3:
					if(language == Chinese)
					{
						lcd_show_string(32, 0, "一氧化碳");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(32, 0, "C0");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
					
					
					temp[0] = '0' + co_LAlarm / 10;
					temp[1] = '0' + co_LAlarm %10;
					temp[2] = '\0';
					
					
					lcd_draw_string(104, 2, "PPM");
					lcd_draw_string(88, 2, temp);
					
				  temp[0] = '0' + co_HAlarm / 100;
					temp[1] = '0' + co_HAlarm % 100/10;
					temp[2] = '0' + co_HAlarm %10;
					temp[3] = '\0';				
					
					lcd_draw_string(104, 4, "PPM");
					lcd_draw_string(80, 4, temp);
				
				
					
					lcd_draw_string(72, 6, "1000PPM");
					break;
				
				case 4:
					if(language == Chinese)
					{
						lcd_show_string(32, 0, "可燃气体");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(32, 0, "EX");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
					
					
					temp[0] = '0' + fire_LAlarm / 10;
					temp[1] = '0' + fire_LAlarm %10;
					temp[2] = '\0';
					
					
					lcd_draw_string(96, 2, "%LEL");
					lcd_draw_string(80, 2, temp);
					
					temp[0] = '0' + fire_HAlarm /10;
					temp[1] = '0' + fire_HAlarm %10;
					temp[2] = '\0';				
					
					lcd_draw_string(96, 4,"%LEL");
					lcd_draw_string(80, 4, temp);
				
				
					
					lcd_draw_string(72, 6, "100%LEL");
					break;
			}
			break;
							
	 case SUB_MENU_LNGUAGE:
				if(language == Chinese)
				{
					lcd_show_string(32, 0, "语言设置");
					lcd_show_string(0, 6, "向上");
					lcd_show_string(48, 6, "确认");
					lcd_show_string(96, 6, "向下");
				}
				else if(language == English)
				{
					lcd_draw_string(32, 0, "Language");
					lcd_draw_string(0, 6, "Up");
					lcd_draw_string(48, 6, "Enter");
					lcd_draw_string(96, 6, "Down");
				}

	 
				if(La_Story == 0)
				{
					if(language == Chinese)
						lcd_show_string(48, 3, "中文");
					else if(language == English)
						lcd_draw_string(48, 3, "Chinese");
				}
				else
				{
					if(language == Chinese)
						lcd_show_string(48, 3, "英文");
					else if(language == English)
						lcd_draw_string(48, 3, "English");
				}
				
				if((KeyValue == 3 || KeyValue == 2)  && F_NewKey == True)
				{
					F_NewKey = False;
					La_Story = La_Story? 0:1;
				}
				else if(KeyValue == 1 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear_now();
					memset(data_buffer, 0, sizeof(data_buffer));
					SubMenuWorkSt = SUB_SAVE_WINDOWN;
					for(i = 0; i < Sector_Argurment_Num; i++)
					{
						data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
						Delay(2);
					}
					data_buffer[ADD_Language - 1] = La_Story;
					change_flag = language_flag;
				}
				
	 
			break;
				
				
				
				
				
				
			//下一级子菜单相应的操作
	 case SUB_H2S_RECORD:
				if(pop_up_time > 0)
				{
					lcd_show_string(40, 2, "无记录");
				}
				else 
				{
					glcd_clear_now();
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				}
	 
		break;
	 
	 case SUB_02_RECORD:
				if(pop_up_time > 0)
				{
					lcd_show_string(40, 2, "无记录");
				}
				else 
				{
					glcd_clear_now();
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				}
	 
		break;
				
	 case SUB_CO_RECORD:
			if(co_record_num > 0)
			{
				if(another_pop_time <= 0)
				{
						lcd_show_string(0, 0, "一氧化碳");
						lcd_draw_string(64, 2, "PPM");
						lcd_show_string(0, 6, "向上");
						lcd_show_string(48, 6, "选项");
						lcd_show_string(96, 6, "向下");
						
						if((co_record_num - 8) < ADD_C0_BASE)	//记录的指针刚好又从2个扇区开始了
						{
							co_record_num = ADD_C0_BASE;
						}
						
						temp[0] = IapReadByte(sp_record_temp - 8);			//取状态值
						Delay(10);
						temp[1] = IapReadByte(sp_record_temp - 7);			//取浓度值
						Delay(10);
						temp[2] = IapReadByte(sp_record_temp - 6);			//取年
						Delay(10);
						temp[3] = IapReadByte(sp_record_temp - 5);			//取月
						Delay(10);
						temp[4] = IapReadByte(sp_record_temp - 4);			//取日
						Delay(10);
						temp[5] = IapReadByte(sp_record_temp - 3);			//取时
						Delay(10);
						temp[6] = IapReadByte(sp_record_temp - 2);			//取分
						Delay(10);
						temp[7] = IapReadByte(sp_record_temp - 1);			//取秒
						Delay(10);
						
						//第一个参数
						if(temp[0] & (1 << 2))
						{
							lcd_show_string(96, 0, "高报");
						}
						else 
						{
							lcd_show_string(96, 0, "低报");
						}
						//第二个参数
						if(temp[1] == 1000)
						{
							tt_buffer[0] = '0' + 1;
							tt_buffer[1] = '0' + 0;
							tt_buffer[2] = '0' + 0;
							tt_buffer[3] = '0' + 0;
							tt_buffer[4] = '\0';
							lcd_draw_string(32, 2, tt_buffer);
						}
						else if(temp[1] >= 100)
						{
							tt_buffer[0] = '0' + temp[1]/100;
							tt_buffer[1] = '0' + temp[1]%100/10;
							tt_buffer[2] = '0' + temp[1]%10;
							tt_buffer[3] = '\0';
							lcd_draw_string(40, 2, tt_buffer);
						}
						else if(temp[1] >= 10)
						{
							tt_buffer[0] = '0' + temp[1]/10;
							tt_buffer[1] = '0' + temp[1]%10;
							tt_buffer[2] = '\0' ;
							lcd_draw_string(48, 2, tt_buffer);
						}
						else 
						{
							lcd_draw_char(56, 2, '0' + temp[1]);
						}
						
						
						//时间显示
		
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(0,4, temp);
						lcd_draw_char(16, 4,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(19,4, temp);
						lcd_draw_char(35, 4,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(38,4, temp);


						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');

						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);
				}
			}
			else 
			{
				if(pop_up_time > 0)
				{
					lcd_show_string(40, 2, "无记录");
				}
				else 
				{
					glcd_clear_now();
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				}
			}
			
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				if(--record_temp > 0)
				{
					glcd_clear(2, 0, 5, 128);
					sp_record_temp -= 8;
					//SendData((uint8_t)(fire_record_num));
				}
				else 
				{
					glcd_clear_now();
					lcd_show_string(32, 2, "第一条");
					another_pop_time = 1500;
				}
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				if(sp_record_temp == fire_sp)
				{
					glcd_clear_now();
					lcd_show_string(32, 2, "最后一页");
					another_pop_time = 1500;
				}
				else 
				{
					glcd_clear(2, 0, 5, 128);
					sp_record_temp += 8;
					record_temp += 1;
				}	
			}
		 else if(KeyValue == 1 && F_NewKey == True)
		 {
			F_NewKey = False;
			glcd_clear_now();
			SubMenuWorkSt = SUB_CHOSE_END;
			record_point = 0;
		 }
	 
		break;	
	 
	 case SUB_FIRE_RECORD:
		 
			if(fire_record_num > 0)
			{
				if(another_pop_time <= 0)
				{
						lcd_show_string(0, 0, "可燃");
						lcd_draw_string(56, 2, "%LEL");
						lcd_show_string(0, 6, "向上");
						lcd_show_string(48, 6, "选项");
						lcd_show_string(96, 6, "向下");
						
						if((sp_record_temp - 8) < ADD_FIRE_BASE)	//记录的指针刚好又从2个扇区开始了
						{
							sp_record_temp = ADD_C0_BASE;
						}
						
						temp[0] = IapReadByte(sp_record_temp - 8);			//取状态值
						Delay(10);
						temp[1] = IapReadByte(sp_record_temp - 7);			//取浓度值
						Delay(10);
						temp[2] = IapReadByte(sp_record_temp - 6);			//取年
						Delay(10);
						temp[3] = IapReadByte(sp_record_temp - 5);			//取月
						Delay(10);
						temp[4] = IapReadByte(sp_record_temp - 4);			//取日
						Delay(10);
						temp[5] = IapReadByte(sp_record_temp - 3);			//取时
						Delay(10);
						temp[6] = IapReadByte(sp_record_temp - 2);			//取分
						Delay(10);
						temp[7] = IapReadByte(sp_record_temp - 1);			//取秒
						Delay(10);
						
						//第一个参数
						if(temp[0] & (1 << 3))
						{
							lcd_show_string(96, 0, "高报");
						}
						else 
						{
							lcd_show_string(96, 0, "低报");
						}
						//第二个参数
						if(temp[1] == 100)
						{
							tt_buffer[0] = '0' + 1;
							tt_buffer[1] = '0' + 0;
							tt_buffer[2] = '0' + 0;
							tt_buffer[3] = '\0';
							lcd_draw_string(32, 2, tt_buffer);
						}
						else
						{
							tt_buffer[0] = '0' + temp[1]/10;
							tt_buffer[1] = '0' + temp[1]%10;
							tt_buffer[2] = '\0';
							lcd_draw_string(40, 2, tt_buffer);
						}
						//时间显示
					//	memset(tt_buffer,0, sizeof(tt_buffer));
					//	sprintf(tt_buffer,"%d.%d.%d",temp[2], temp[3],temp[4]);
					//	lcd_draw_string(0, 4, tt_buffer);
					//	sprintf(tt_buffer, "%d:%d:%d", temp[5], temp[6], temp[7]);
						//lcd_draw_string(64, 4, tt_buffer);
						
						temp[0] = '0' + Year/10;
						temp[1] = '0' + Year%10;
						temp[2] = '\0';

						lcd_draw_string(0,4, temp);
						lcd_draw_char(16, 4,'.');

						temp[0] = '0' + Month/10;
						temp[1] = '0' + Month%10;
						temp[2] = '\0';
						
						lcd_draw_string(19,4, temp);
						lcd_draw_char(35, 4,'.');

						temp[0] = '0'+Day/10;
						temp[1] = '0'+Day%10;
						temp[2] = '\0';
						
						lcd_draw_string(38,4, temp);


						temp[0] = '0'+Hour/10;
						temp[1] = '0'+Hour%10;
						temp[2] = '\0';
						lcd_draw_string(64,4, temp);
						lcd_draw_char(80,4,':');

						temp[0] = '0'+Minute/10;
						temp[1] = '0'+Minute%10;
						temp[2] = '\0';
						lcd_draw_string(88,4, temp);
						lcd_draw_char(104, 4,':');

						temp[0] = '0'+Seond/10;
						temp[1] = '0'+Seond%10;
						temp[2] = '\0';
						lcd_draw_string(112,4, temp);
				}
			}
			else 
			{
				if(pop_up_time > 0)
				{
					lcd_show_string(40, 2, "无记录");
				}
				else 
				{
					glcd_clear_now();
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				}
			}
			
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				if(--record_temp > 0)
				{
					glcd_clear(2, 0, 5, 128);
					sp_record_temp -= 8;
					//SendData((uint8_t)(fire_record_num));
				}
				else 
				{
					glcd_clear_now();
					lcd_show_string(32, 2, "最后一页");
					another_pop_time = 1500;
				}
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				if(sp_record_temp == fire_sp)
				{
					glcd_clear_now();
					lcd_show_string(32, 2, "最后一页");
					another_pop_time = 1500;
				}
				else 
				{
					glcd_clear(2, 0, 5, 128);
					sp_record_temp += 8;
					record_temp += 1;
				}	
			}
		 else if(KeyValue == 1 && F_NewKey == True)
		 {
			F_NewKey = False;
			glcd_clear_now();
			SubMenuWorkSt = SUB_CHOSE_END;
			record_point = 0;
		 }
		
		break;			
			
	 
			

		case SUB_LOWALARM_HySet:
			
			temp[0] = '0' + sub_temp / 10;
			temp[1] = '0' + sub_temp %10;
			temp[2] = '\0';
			lcd_draw_string(80, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "低报设置");
				lcd_show_string(0, 3, "硫化氢");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set Low");
				lcd_draw_string(0, 3, "H2S");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			
			lcd_draw_string(96, 3, "PPM");
			
			if(KeyValue == 1 && F_NewKey == True)	//确认键
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_hydrothion_LAlarm - 1] = sub_temp;
			}
			else if(KeyValue == 2 && F_NewKey == True)	//减少
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp > 0)
						sub_temp--;
			}
			else if(KeyValue == 3 && F_NewKey == True)	//增加
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp  < 20)
						sub_temp++;
			}
			break;
		
		case SUB_LOWALARM_O2Set:
			sprintf(temp,"%.1f",sub_O2_temp);
			lcd_draw_string(64, 3, temp);
			
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "低报设置");
				lcd_show_string(0, 3, "氧气");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set Low");
				lcd_draw_string(0, 3, "O2");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_char(96, 3, '%');
		
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				if(sub_O2_temp < 20.1)
					sub_O2_temp = sub_O2_temp + 0.1;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				if(sub_O2_temp > 18.0)
					sub_O2_temp = sub_O2_temp - 0.1;
			}
			else if(KeyValue == 1 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				p_temp = (uint8_t *)&sub_O2_temp;
				for(i = 0; i < 4; i++)
				{
					data_buffer[ADD_o2_LAlarm_Base - 1 + i] = *(p_temp + i);
				}
							
			}
			
			break;
	

	case SUB_LOWALARM_CoSet:
			
			temp[0] = '0' + sub_temp / 10;
			temp[1] = '0' + sub_temp %10;
			temp[2] = '\0';
			lcd_draw_string(80, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "低报设置");
				lcd_show_string(0, 3, "一氧化碳");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set Low");
				lcd_draw_string(0, 3, "CO");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_string(96, 3, "PPM");
			
			if(KeyValue == 1 && F_NewKey == True)	//确认键
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_co_LAlarm - 1] = sub_temp;
			}
			else if(KeyValue == 2 && F_NewKey == True)	//减少
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp > 0)
						sub_temp--;
			}
			else if(KeyValue == 3 && F_NewKey == True)	//增加
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp  < 20)
						sub_temp++;
			}
			break;


	case SUB_LOWALARM_FireSet:
			
			temp[0] = '0' + sub_temp / 10;
			temp[1] = '0' + sub_temp %10;
			temp[2] = '\0';
			lcd_draw_string(80, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "低报设置");
				lcd_show_string(0, 3, "可燃");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set Low");
				lcd_draw_string(0, 3, "EX");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_string(96, 3, "%LEL");
			
			if(KeyValue == 1 && F_NewKey == True)	//确认键
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_fire_LAlarm - 1] = sub_temp;
			}
			else if(KeyValue == 2 && F_NewKey == True)	//减少
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp > 0)
						sub_temp--;
			}
			else if(KeyValue == 3 && F_NewKey == True)	//增加
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp  < 20)
						sub_temp++;
			}
			break;
////////////////////////////////////////////////////////////
			
			
			
	case SUB_HIGHTALARM_HySet:
			
			temp[0] = '0' + sub_temp / 10;
			temp[1] = '0' + sub_temp %10;
			temp[2] = '\0';
			lcd_draw_string(80, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "高报设置");
				lcd_show_string(0, 3, "硫化氢");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set High");
				lcd_draw_string(0, 3, "H2S");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_string(96, 3, "PPM");
			
			if(KeyValue == 1 && F_NewKey == True)	//确认键
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_hydrothion_HAlarm - 1] = sub_temp;
			}
			else if(KeyValue == 2 && F_NewKey == True)	//减少
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp > 0)
						sub_temp--;
			}
			else if(KeyValue == 3 && F_NewKey == True)	//增加
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp  < 20)
						sub_temp++;
			}
			break;
		
		case SUB_HIGHTALARM_O2Set:
			sprintf(temp,"%.1f",sub_O2_temp);
			lcd_draw_string(64, 3, temp);
				
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "高报设置");
				lcd_show_string(0, 3, "氧气");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set High");
				lcd_draw_string(0, 3, "O2");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_char(96, 3, '%');
		
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				if(sub_O2_temp < 20.1)
					sub_O2_temp = sub_O2_temp + 0.1;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				if(sub_O2_temp > 18.0)
					sub_O2_temp = sub_O2_temp - 0.1;
			}
			else if(KeyValue == 1 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				p_temp = (uint8_t *)&sub_O2_temp;
				for(i = 0; i < 4; i++)
				{
					data_buffer[ADD_o2_HAlarm_Base - 1 + i] = *(p_temp + i);
				}
							
			}
			
			break;
	

	case SUB_HIGHTALARM_CoSet:
			
			temp[0] = '0' + sub_temp / 10;
			temp[1] = '0' + sub_temp %10;
			temp[2] = '\0';
			lcd_draw_string(80, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "高报设置");
				lcd_show_string(0, 3, "一氧化碳");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set High");
				lcd_draw_string(0, 3, "CO");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_string(96, 3, "PPM");
			
			if(KeyValue == 1 && F_NewKey == True)	//确认键
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_co_HAlarm - 1] = sub_temp;
			}
			else if(KeyValue == 2 && F_NewKey == True)	//减少
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp > 0)
						sub_temp--;
			}
			else if(KeyValue == 3 && F_NewKey == True)	//增加
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp  < 20)
						sub_temp++;
			}
			break;


	case SUB_HIGHTALARM_FireSet:
			
			temp[0] = '0' + sub_temp / 10;
			temp[1] = '0' + sub_temp %10;
			temp[2] = '\0';
			lcd_draw_string(80, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "高报设置");
				lcd_show_string(0, 3, "可燃");
				lcd_show_string(0, 6, "增加");
				lcd_show_string(48, 6, "确认");
				lcd_show_string(96, 6, "减少");
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Set High");
				lcd_draw_string(0, 3, "EX");
				lcd_draw_string(0, 6, "Up");
				lcd_draw_string(48, 6, "Enter");
				lcd_draw_string(96, 6, "Down");
			}
			lcd_draw_string(96, 3, "%LEL");
			
			if(KeyValue == 1 && F_NewKey == True)	//确认键
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_fire_HAlarm - 1] = sub_temp;
			}
			else if(KeyValue == 2 && F_NewKey == True)	//减少
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp > 0)
						sub_temp--;
			}
			else if(KeyValue == 3 && F_NewKey == True)	//增加
			{
				F_NewKey = False;
				glcd_clear(3, 80, 4, 96);
				if(sub_temp  < 20)
						sub_temp++;
			}
			break;
	//////////////////////////////////////////
			
	case SUB_ZERO_HySet:
			sprintf(temp, "%d", hydrothion_gas_adc);
			lcd_draw_string(56, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "零点设置");
				lcd_show_string(0, 6, "保存");
				lcd_show_string(96, 6, "退出");	
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Zero Adjust");
				lcd_draw_string(0, 6, "Enter");
				lcd_draw_string(96, 6, "Exit");	
			}

		
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_hydrothion_ZERO - 1] = (uint8_t)(hydrothion_gas_adc >> 8);
				data_buffer[ADD_hydrothion_ZERO] = 	(uint8_t)hydrothion_gas_adc;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
			}
			break;
			
		
	case SUB_ZERO_O2Set:
			sprintf(temp, "%d", o2_gas_adc);
			lcd_draw_string(56, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "零点设置");
				lcd_show_string(0, 6, "保存");
				lcd_show_string(96, 6, "退出");	
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Zero Adjust");
				lcd_draw_string(0, 6, "Enter");
				lcd_draw_string(96, 6, "Exit");	
			}
		
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_o2_ZERO - 1] = (uint8_t)(o2_gas_adc >> 8);
				data_buffer[ADD_o2_ZERO] = 	(uint8_t)o2_gas_adc;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
			}
			break;	
		
	case SUB_ZERO_CoSet:
			sprintf(temp, "%d", co_gas_adc);
			lcd_draw_string(56, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "零点设置");
				lcd_show_string(0, 6, "保存");
				lcd_show_string(96, 6, "退出");	
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Zero Adjust");
				lcd_draw_string(0, 6, "Enter");
				lcd_draw_string(96, 6, "Exit");	
			}

		
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_co_ZERO - 1] = (uint8_t)(co_gas_adc >> 8);
				data_buffer[ADD_co_ZERO] = 	(uint8_t)co_gas_adc;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
			}
			break;				
	
	case SUB_ZERO_FireSet:
			sprintf(temp, "%d", fire_gas_adc);
			lcd_draw_string(56, 3, temp);
		
			if(language == Chinese)
			{
				lcd_show_string(32, 0, "零点设置");
				lcd_show_string(0, 6, "保存");
				lcd_show_string(96, 6, "退出");	
			}
			else if(language == English)
			{
				lcd_draw_string(32, 0, "Zero Adjust");
				lcd_draw_string(0, 6, "Enter");
				lcd_draw_string(96, 6, "Exit");	
			}

		
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				memset(data_buffer, 0, sizeof(data_buffer));
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
					data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
					Delay(2);
				}
				data_buffer[ADD_fire_ZERO - 1] = (uint8_t)(fire_gas_adc >> 8);
				data_buffer[ADD_fire_ZERO] = 	(uint8_t)fire_gas_adc;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
			}
		 break;			
			

	case SUB_PASSWORK_CHANGE:
			
		if(language == Chinese)
		{
			lcd_show_string(32, 0, "密码验证");
			lcd_show_string(0, 6, "增加");
			lcd_show_string(48, 6, "确认");
			lcd_show_string(96, 6, "减少");
		}
		else if(language == English)
		{
			lcd_draw_string(32, 0, "New Password");
			lcd_draw_string(0, 6, "Up");
			lcd_draw_string(48, 6, "Enter");
			lcd_draw_string(96, 6, "Down");
		}

		


		switch(sub_point)
		{
			case 0:
				lcd_shadow_draw_char(32,3,'0' + PassTemp[0]);
				lcd_draw_char(48,3,'0' + PassTemp[1]);
				lcd_draw_char(64,3,'0' + PassTemp[2]);
				lcd_draw_char(80,3,'0' + PassTemp[3]);
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					PassTemp[0]++;
					if(PassTemp[0] >= 10)
					{
						PassTemp[0] = 0;
					}	
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					if(PassTemp[0] <= 0)
							PassTemp[0] = 9;
					else
						PassTemp[0]--;
				}
				break;
			
			case 1:
				lcd_draw_char(32,3,'0'+ PassTemp[0]);
				lcd_shadow_draw_char(48,3,'0'+ PassTemp[1]);
				lcd_draw_char(64,3,'0'+ PassTemp[2]);
				lcd_draw_char(80,3,'0'+ PassTemp[3]);
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					PassTemp[1]++;
					if(PassTemp[1] >= 10)
					{
						PassTemp[1] = 0;
					}	
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					if(PassTemp[1] <= 0)
							PassTemp[1] = 9;
					else
						PassTemp[1]--;
				}
			
			
				break;
			
			case 2:
				lcd_draw_char(32,3,'0'+ PassTemp[0]);
				lcd_draw_char(48,3,'0'+ PassTemp[1]);
				lcd_shadow_draw_char(64,3,'0'+ PassTemp[2]);
				lcd_draw_char(80,3,'0'+ PassTemp[3]);
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					PassTemp[2]++;
					if(PassTemp[2] >= 10)
					{
						PassTemp[2] = 0;
					}	
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					if(PassTemp[2] <= 0)
							PassTemp[2] = 9;
					else
						PassTemp[2]--;
				}
				break;
			
			case 3:
				lcd_draw_char(32,3,'0'+ PassTemp[0]);
				lcd_draw_char(48,3,'0'+ PassTemp[1]);
				lcd_draw_char(64,3,'0'+ PassTemp[2]);
				lcd_shadow_draw_char(80,3,'0'+ PassTemp[3]);
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					PassTemp[3]++;
					if(PassTemp[3] >= 10)
					{
						PassTemp[3] = 0;
					}	
				}
				else if(KeyValue == 2 && F_NewKey == True)
				{
					F_NewKey = False;
					if(PassTemp[3] <= 0)
							PassTemp[3] = 9;
					else
						PassTemp[3]--;
				}					
				break;
			
			default:
				break;
		}
		
		if(KeyValue == 1 && F_NewKey == True)
		{
			F_NewKey = False;
			sub_point++;	
			if(sub_point >= 4)
			{
				glcd_clear_now();
				SubMenuWorkSt = SUB_SAVE_WINDOWN;
				for(i = 0; i < Sector_Argurment_Num; i++)
				{
						data_buffer[i] = IapReadByte(ADD_BASE_NUM+i);
						Delay(2);
				}
				for(i = 0; i < 4; i++)
				{
					data_buffer[ADD_PassWork_Base - 1 + i] = PassTemp[i];
				}
				change_flag = passwork_flag;
			}
		}

		break;


	case SUB_CHOSE_END:
			
			lcd_show_string(48, 0, "选项");
			lcd_show_string(16, 2, "返回");
			lcd_show_string(16, 4, "删除");
			lcd_show_string(16, 6, "退出");
			
			switch(record_point)
			{
				case 0:
					lcd_draw_char(8, 2, '>');
					break;
				case 1:
					lcd_draw_char(8, 4, '>');
					break;
				case 2:
					lcd_draw_char(8, 6, '>');
					break;
			}
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear(2,0, 7,16);
				if(record_point <= 0)
				{
					record_point = 2;
				}
				else
				{
					record_point--;
				}
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear(2,0, 7,16);
				record_point++;
				if(record_point >= 3)
					record_point = 0;
			}
			else if(KeyValue == 1 && F_NewKey == True)
			{
				F_NewKey = False;
				switch(record_point)
				{
					case 0:
								glcd_clear_now();
								if(sub_point == 3)
									SubMenuWorkSt = SUB_FIRE_RECORD;
								else if(sub_point == 2)
									SubMenuWorkSt = SUB_CO_RECORD;
								else if(sub_point == 1)
									SubMenuWorkSt = SUB_02_RECORD;
								else if(sub_point == 0)
									SubMenuWorkSt = SUB_H2S_RECORD;
						break;
					
					case 1:
						SubMenuWorkSt = SUB_RECODR_DELECT;
						glcd_clear_now();
						break;
					
					case 2:
							glcd_clear_now();
							SubMenuWorkSt = SUB_MENU_IDLE;
							SystemWorkSt = NORMAL_WORK_PAGE;
						break;
				}
			}
		break;
			
			
	case SUB_SAVE_WINDOWN:
			//界面
			if(language == Chinese)
			{
				lcd_show_chinese(0, 6, "是");
				lcd_show_chinese(112, 6, "否");
				lcd_show_string(48, 3, "保存");
			}
			else if(language == English)
			{
				lcd_draw_string(0, 6, "YES");
				lcd_draw_string(112, 6, "NO");
				lcd_draw_string(48, 3, "Save");
			}

			lcd_draw_char(80, 3, '?');
			
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
				
				
				if(sub_point >= 6)
				{
					Set1302();
					original_month = Month;
					original_day = Day;
					original_houre = Hour;
					original_minute = Minute;
				}
				else
				{
					IapEraseSector(0x0000);
					for(i = 0; i < Sector_Argurment_Num; i++)
					{
						IapProgramByte(ADD_BASE_NUM + i, data_buffer[i]);
						Delay(5);
					}
					
					switch(change_flag)
					{
						case hydrothion_LAlarm_flag:
							hydrothion_LAlarm = sub_temp;
						 break;
						case fire_LAlarm_flag:
							fire_LAlarm = sub_temp;
							break;
						case o2_LAlarm_flag:
							o2_LAlarm.x = sub_O2_temp ;
							break;
						case co_LAlarm_flag:
							co_LAlarm = sub_temp;
							break;
						case hydrothion_HAlarm_flag:
							hydrothion_HAlarm = sub_temp;
							break;
						case fire_HAlarm_flag:
							fire_HAlarm = sub_temp;
							break;
						case o2_HAlarm_flag:
							o2_HAlarm.x = sub_O2_temp ;
							break;					
						case co_HAlarm_flag:
							co_HAlarm = sub_temp;
							break;
						case backlight_arg_flag:
								backlight_arg = TS_Story;
							break;
						case passwork_flag:
							for(i = 0; i < 4; i++)
							{
								PassWord[i] = PassTemp[i];
							}
							break;
						case language_flag:
							language = La_Story;
						default:
							break;
					}
				}
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
					
			}
			break;
			
			
		case SUB_RECODR_DELECT:

				switch(sub_point)
				{
					case 0:
						lcd_show_string(32, 0, "硫化氢");
						break;
					
					case 1:
						lcd_show_string(32, 0, "氧气");
						break;
					
					case 2:
						lcd_show_string(32, 0, "一氧化碳");
					
						break;
					case 3:
						lcd_show_string(32, 0, "可燃气体");
						break;
				}
					
				
			lcd_show_string(8, 2, "该记录全部删除");
			lcd_draw_char(120, 2, '?');
			lcd_show_chinese(0, 6, "是");
			lcd_show_chinese(112, 6, "否");
				
				
			if(KeyValue == 3 && F_NewKey == True)
			{
				F_NewKey = False;
				switch(sub_point)
				{
					case 0:
						for(i = 0; i < 5; i++)									//硫化氢有5个扇区
							{
								IapEraseSector(ADD_H2S_BASE + (i*512));
							}
						h2s_sp = ADD_H2S_BASE;
						h2s_record_num = 0;	
						for(i = 0; i < Record_Argurment_Num;i++)
							{
								temp[i] = IapReadByte(ADD_FIRE_SP_BASE+ i);
							}
						IapEraseSector(ADD_FIRE_SP_BASE);
						temp[ADD_H2S_NUM]	= (uint8_t)(h2s_sp >> 8);
						temp[ADD_H2S_NUM + 1] = (uint8_t)h2s_sp;
						temp[ADD_H2S_COUNT] = 0;
						temp[ADD_H2S_COUNT + 1] = 0;
						for(i = 0; i < Record_Argurment_Num; i++)
						{
							IapProgramByte(ADD_FIRE_SP_BASE + i, temp[i]);
						}
						
						break;
					
					case 1:
						for(i = 0; i < 7; i++)									//氧气有7个扇区
							{
								IapEraseSector(ADD_O2_BASE + (i*512));
							}
						o2_sp = ADD_O2_BASE;
						o2_record_num = 0;	
						for(i = 0; i < Record_Argurment_Num;i++)
							{
								temp[i] = IapReadByte(ADD_FIRE_SP_BASE+ i);
							}
						IapEraseSector(ADD_FIRE_SP_BASE);
						temp[ADD_02_NUM]	= (uint8_t)(o2_sp >> 8);
						temp[ADD_02_NUM + 1] = (uint8_t)o2_sp;
						temp[ADD_02_COUNT] = 0;
						temp[ADD_02_COUNT + 1] = 0;
						for(i = 0; i < Record_Argurment_Num; i++)
						{
							IapProgramByte(ADD_FIRE_SP_BASE + i, temp[i]);
						}
				
						break;
					
					case 2:
						for(i = 0; i < 5; i++)									//一氧化碳有5个扇区
							{
								IapEraseSector(ADD_C0_BASE + (i*512));
							}
						co_sp = ADD_C0_BASE;
						co_record_num = 0;	
						for(i = 0; i < Record_Argurment_Num;i++)
							{
								temp[i] = IapReadByte(ADD_FIRE_SP_BASE+ i);
							}
						IapEraseSector(ADD_FIRE_SP_BASE);
						temp[ADD_C0_NUM]	= (uint8_t)(co_sp >> 8);
						temp[ADD_C0_NUM + 1] = (uint8_t)co_sp;
						temp[ADD_C0_COUNT] = 0;
						temp[ADD_C0_COUNT + 1] = 0;
						for(i = 0; i < Record_Argurment_Num; i++)
						{
							IapProgramByte(ADD_FIRE_SP_BASE + i, temp[i]);
						}
						break;
						
					case 3:
						for(i = 0; i < 5; i++)									//可燃气体有5个扇区
							{
								IapEraseSector(ADD_FIRE_BASE + (i*512));
							}
						fire_sp = ADD_FIRE_BASE;
						fire_record_num = 0;	
						for(i = 0; i < Record_Argurment_Num;i++)
							{
								temp[i] = IapReadByte(ADD_FIRE_SP_BASE+ i);
							}
						IapEraseSector(ADD_FIRE_SP_BASE);
						temp[ADD_FIRE_NUM]	= (uint8_t)(fire_sp >> 8);
						temp[ADD_FIRE_NUM + 1] = (uint8_t)fire_sp;
						temp[ADD_FIRE_COUNT] = 0;
						temp[ADD_FIRE_COUNT + 1] = 0;
						for(i = 0; i < Record_Argurment_Num; i++)
						{
							IapProgramByte(ADD_FIRE_SP_BASE + i, temp[i]);
						}
						break;
					
				}
					glcd_clear_now();
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				glcd_clear_now();
				SubMenuWorkSt = SUB_MENU_IDLE;
				SystemWorkSt = NORMAL_WORK_PAGE;
			}
				
				break;
			
					
				
			default:
				break;
			
	}
}







//密码进入处理部分
void Handle_PassWork(void)
{	
	switch(PassWordChange_ST)
		{
			case 0 : 
					 
					lcd_shadow_draw_char(32,4,'0' + PassTemp[0]);
					lcd_draw_char(48,4,'0' + PassTemp[1]);
					lcd_draw_char(64,4,'0' + PassTemp[2]);
					lcd_draw_char(80,4,'0' + PassTemp[3]); 
					 
					 if(KeyValue == 1 && 	F_NewKey == True)
					 {
						 F_NewKey = False;
						 
						 if(PassTemp[0] !=PassWord[0])
						 {
								PassFlag = 1;
						 }
						 PassWordChange_ST = 1;	
					 }
					 if(KeyValue == 2&& F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[0] > 0)
							 PassTemp[0]--; 
					 }
					 if(KeyValue == 3 && F_NewKey == True)
					 {
							F_NewKey = False;
							if(PassTemp[0] < 9)
								PassTemp[0]++;
					 }
				 break;

			case 1 :
					 lcd_draw_char(32,4,'0' + PassTemp[0]);
					 lcd_shadow_draw_char(48,4,'0' + PassTemp[1]);
					 lcd_draw_char(64,4,'0' + PassTemp[2]);
					 lcd_draw_char(80,4,'0' + PassTemp[3]); 
					 if(KeyValue == 1 && F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[1] !=PassWord[1])
						 {
								PassFlag = 1;
						 }
						 PassWordChange_ST = 2;	
					 }
					 if(KeyValue == 2 && F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[1] > 0)
							 PassTemp[1]--; 
					 }
					 if(KeyValue == 3 && F_NewKey == True)
					 {
						 F_NewKey = False;
							if(PassTemp[1] < 9)
								PassTemp[1]++;
					 }
	
				 break;

					 
			case 2 :
					 lcd_draw_char(32,4,'0' + PassTemp[0]);
					 lcd_draw_char(48,4,'0' + PassTemp[1]);
					 lcd_shadow_draw_char(64,4,'0' + PassTemp[2]);
					 lcd_draw_char(80,4,'0' + PassTemp[3]); 
				
					 if(KeyValue == 1&& F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[2] !=PassWord[2])
						 {
								PassFlag = 1;
						 }
						 PassWordChange_ST = 3;	
					 }
					 if(KeyValue == 2 && F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[2] > 0)
							 PassTemp[2]--; 
					 }
					 if(KeyValue == 3 && F_NewKey == True)
					 {
						 F_NewKey = False;
							if(PassTemp[2] < 9)
								PassTemp[2]++;
					 }
						
				 break;
				 
			case 3 :
				   lcd_draw_char(32,4,'0' + PassTemp[0]);
					 lcd_draw_char(48,4,'0' + PassTemp[1]);
					 lcd_draw_char(64,4,'0' + PassTemp[2]);
					 lcd_shadow_draw_char(80,4,'0' + PassTemp[3]); 
			
					 if(KeyValue == 1 && F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[3] !=PassWord[3])
						 {
								PassFlag = 1;
						 }
						 PassWordChange_ST = 4;	
					 }
					 if(KeyValue == 2 && F_NewKey == True)
					 {
						 F_NewKey = False;
						 if(PassTemp[3] > 0)
							 PassTemp[3]--; 
					 }
					 if(KeyValue == 3 && F_NewKey == True)
					 { 
							F_NewKey = False;
							if(PassTemp[3] < 9)
								PassTemp[3]++;
					 }
						
		 break;
 
			case 4 :

				   if(PassFlag ==0)
					   {
							  SystemWorkSt = MENU_SET;
							 	PassFlag = 0;
								memset(PassTemp, 0, sizeof(PassTemp));
								SystemWorkSt = MENU_SET;
								PassWordChange_ST = 0;
					   }
				   else
					   {
					   	  //切换回主菜单
								
							/*********语音播报******/
							 reset();
							 send(sound_errorcode);
							/***********************/
							 PassWordChange_ST = 0;
							 SystemWorkSt = NORMAL_WORK_PAGE;
							 PassWordChange_ST = 0;
					   }
					glcd_clear_now();
						 
				 break;	 

			default :
				 PassWordChange_ST = 0;
				 break;
				 
		}
}

/*******************************
*	函数功能：系统初始化流程
*			
*			描述: 将系统工作划分为 
*														 硬件的初始化，
*														 系统数据的初始化，
********************************/
void System_Init(void)
{
	uint16_t lltemp;
	uint8_t temp;
	uint8_t i;
	
	union
	{
		float x;
		uint8_t s[4];
	}tt;	
/**********系统硬件部分*******/
	
	
	
	
/************end*************/
	
	
	
/**********默认参数部分*******/
	falue_flag = 0;			//异常报错状态值
	Low_alarm_flag = 0;	//低浓度报警状态值
	High_alarm_flag = 0;	//高浓度报警状态值
	Handle_sound_flag = 0; //4路声音处理标志位
	
	sound_chose = 0;
	sum_minute = 0;
	original_month = 0;
	original_day = 0;
	original_houre = 0;
	original_minute = 0;
	
	
	
	
	max_fire = 0;		//报警过程中最大的数值
	max_co = 0;		
	max_h2s = 0;		
	max_o2 = 0;
	min_o2 = 0; 
	record_statue	= 0;
	
	fflag = 0;//振动的状态转换
	
	
	
	hydrothion_LAlarm = 10;
	fire_LAlarm = 10;
	o2_LAlarm.x =	18.8;
	co_LAlarm = 10;
	
	hydrothion_HAlarm = 35;
	fire_HAlarm = 50;
	co_HAlarm = 150;
	o2_HAlarm.x = 23.5;
	
	original_hydrothion_adc = 0;
	original_fire_adc = 193	;
	original_co_adc = 0;
	original_o2_adc = 366;
	
	backlight_arg = 0;
	language = 0;
	
	
	fire_sp = ADD_FIRE_BASE;
	co_sp = ADD_C0_BASE;
	h2s_sp = ADD_H2S_BASE;
	o2_sp = ADD_O2_BASE;
	
	fire_record_num = 0;
	co_record_num = 0;
	h2s_record_num = 0;
	o2_record_num = 0;
	
/************end*************/	

///////////////////低报预警内存提取/////////////
	temp = IapReadByte(ADD_hydrothion_LAlarm);
  if(temp!= 0xff)
  {
    hydrothion_LAlarm = temp;
  }
	
	temp = IapReadByte(ADD_fire_LAlarm);
  if(temp!=0xff)
  {
    fire_LAlarm = temp;
  }
	
	temp = IapReadByte(ADD_co_LAlarm);
  if(temp!= 0xff)
  {
    co_LAlarm = temp;
  }
	
	for(i = 0; i< 4; i++)
	{
		tt.s[i] = IapReadByte(ADD_o2_LAlarm_Base + i);		
	}
	if(tt.x > 16.0  && tt.x < 19.0)
	{
		o2_LAlarm.x = tt.x ;
	}
	//////////////////高报预警内存提取/////////////
	temp = IapReadByte(ADD_hydrothion_HAlarm);
  if(temp!=0xff)
  {
    hydrothion_HAlarm = temp;
  }
	
	temp = IapReadByte(ADD_fire_HAlarm);
  if(temp != 0xff)
  {
    fire_HAlarm = temp;
  }
	
	temp = IapReadByte(ADD_co_HAlarm);
  if(temp!=0xff)
  {
    co_HAlarm = temp;
  }
	
	for(i = 0; i< 4; i++)
	{
		tt.s[i] = IapReadByte(ADD_o2_HAlarm_Base + i);		
	}
	if(tt.x > 18.1 && tt.x < 30.0)
	{
		o2_HAlarm.x = tt.x ;
	}
	
	//////////////////原始ADC值内存提取/////////////
	temp = IapReadByte(ADD_hydrothion_ZERO);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_hydrothion_ZERO + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		original_hydrothion_adc = lltemp;
	}
	
	
	temp = IapReadByte(ADD_fire_ZERO);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_fire_ZERO + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		original_fire_adc = lltemp;
	}
	
	temp = IapReadByte(ADD_co_ZERO);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_co_ZERO + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		original_co_adc = lltemp;
	}
	
	temp = IapReadByte(ADD_o2_ZERO);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_o2_ZERO + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		original_o2_adc = lltemp;
	}
	//////////////////背光参数内存提取/////////////
	temp = IapReadByte(ADD_Backlight_Arg);
	if(temp != 0xff)
	{
		backlight_arg = temp;
	}
	//////////////////语言选择内存提取/////////////
	temp = IapReadByte(ADD_Language);
	if(temp !=  0xff)
	{
		language = temp;
	}
	
	////////////////报警记录位置信息提取/////////////
	
	//可燃
	temp = IapReadByte(ADD_FIRE_SP_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_FIRE_SP_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		fire_sp = lltemp;
	}
	//一氧化碳
	temp = IapReadByte(ADD_C0_SP_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_C0_SP_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		co_sp = lltemp;
	}
	
	//硫化氢
	temp = IapReadByte(ADD_H2S_SP_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_H2S_SP_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		h2s_sp = lltemp;
	}
	//氧气
	temp = IapReadByte(ADD_O2_SP_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_O2_SP_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		o2_sp = lltemp;
	}
	
	//可燃气体的异常记录数量
	temp = IapReadByte(ADD_FIRE_COUNT_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_FIRE_COUNT_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		fire_record_num = lltemp;
	}
	//一氧化碳气体异常记录的数量
	temp = IapReadByte(ADD_C0_COUNT_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_C0_COUNT_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		co_record_num = lltemp;
	}
	
	//硫化氢气体异常记录的数量
	temp = IapReadByte(ADD_H2S_COUNT_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_H2S_COUNT_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		h2s_record_num = lltemp;
	}
	
	//氧气异常记录的数量
	temp = IapReadByte(ADD_O2_COUNT_BASE);
	lltemp = (uint16_t)(temp << 8);
	temp = IapReadByte(ADD_O2_COUNT_BASE + 1);
	lltemp |= (uint16_t)temp;
	if(lltemp != 0xffff)
	{
		o2_record_num = lltemp;
	}

	
	//获取原始时间
	Update_DateAndTime();
	original_month = Month;
	original_day = Day;
	original_houre = Hour;
	original_minute = Minute;
}

/*******************************
*	函数功能：系统启动界面
*			
*			描述: 出现的问题 led灯和振动未能按预期的动作
*						在主循环外面有对LED 和振动器的相关操作,加个条件判断就可以了
*				
********************************/

void self_check(void)
{		
	static uint8_t ffcount = 0;
	uint8_t temp[10] = {0};
	static uint8_t ffnum = 0;
	
	switch(Selfcheck_System)
	{
		case ON_IDLE:
				reset();
				send(sound_welcom);
				while(busy);
				begin_time = 2800;
			 
		
				Selfcheck_System = WELCOM;
			break;
		
		case WELCOM:
				if(shake_num <=  0 && ffcount < 4)
				{
					ffcount += 1;
					shake_num = 80;
					LED_1 = ~LED_1;
					if(fflag == 0)
					{
						Shake_Pin(1);
						fflag = 1;
					}
					else 
					{
						Shake_Pin(0);
						fflag = 0;
					}
				}
		
				lcd_show_string(32, 2, "气体探测器");
				lcd_draw_string(56, 4, "Ver 1.1.0");
				

				
				if(begin_time <= 0)
				{
					fflag = 0;
					Shake_Pin(0);
					LED_1 = 0;
					glcd_clear_now();
					Selfcheck_System = ISBEGINNING;	
					reset();
					send(sound_powerup);
				//	while(busy);
					begin_time = 6500;
					ffcount = 0;
					lcd_draw_bar(16, 7, 98);
				}
									
			break;
		
		case ISBEGINNING:
				lcd_show_string(32, 2, "正在启动");
				lcd_show_string(64, 4, "请稍候");
				
			
				if(begin_time > 0 && ffcount <= 98)
				{
					ffnum += 1;
					bar_slider(16 + ffcount, 7);
					if(ffnum >= 5)
					{
						ffcount ++;
						ffnum = 0;
					}
				}
				
		
		
					
				if(begin_time <= 0 || ffcount == 98)
				{
					Selfcheck_System = SELF_CHECK;	
					begin_time = 6000;
					glcd_clear_now();
					lcd_draw_bar(16, 7, 98);
					ffcount = 0;
					ffnum = 0;
				}
			break;
		
		case SELF_CHECK:
			
			lcd_show_string(32, 0, "开机自检");
			if(begin_time >= 3000)
			{
					lcd_show_string(48, 4, "振动");
					if(shake_num <= 0)
					{
						shake_num = 500;
						if(fflag == 0)
						{
							Shake_Pin(1);
							fflag = 1;
						}
						else 
						{
							Shake_Pin(0);
							fflag = 0;
						}							
					}
			}
			else
			{
					Shake_Pin(0);
					lcd_show_string(40, 4, "报警灯");
					if(shake_num <= 0)
					{
						shake_num = 500;
						LED_1 = ~LED_1;
					}
			}
			
				if(begin_time > 3000 && ffcount <= 49)
				{
					ffnum++;
					bar_slider(16 + ffcount, 7);
					if(ffnum >= 4)
					{
						ffcount ++;
						ffnum = 0;
					}	
				}
				else if(begin_time < 3000 && ffcount <= 98)
				{
					ffnum ++;
					bar_slider(16 + ffcount, 7);
					if(ffnum >= 4)
					{
						ffcount ++;
						ffnum = 0;
					}
				}
			
			if(begin_time <= 0 )
			{
				Selfcheck_System =	CHANEL_MEG;
				glcd_clear_now();
			}
		
			break;
		
		case CHANEL_MEG:
			
			if(tnum <= 0)
			{
				memset(temp, 0, sizeof(temp));
				tnum = 3000;
				sub_point++;
				glcd_clear_now();
				if(sub_point >= 5)
				{
					SubMenuWorkSt = SUB_MENU_IDLE;
					SystemWorkSt = NORMAL_WORK_PAGE;
				}
			}
			
			switch(sub_point)
			{
				case 1:
					
					if(language == Chinese)
					{
						lcd_show_string(40, 0, "硫化氢");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(40, 0, "H2S");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
					temp[0] = '0' + hydrothion_LAlarm / 10;
					temp[1] = '0' + hydrothion_LAlarm %10;
					temp[2] = '\0';
	
					lcd_draw_string(104, 2, "PPM");
					lcd_draw_string(88, 2, temp);
					
				  temp[0] = '0' + hydrothion_HAlarm / 10;
					temp[1] = '0' + hydrothion_HAlarm %10;
					temp[2] = '\0';				
					lcd_draw_string(104, 4, "PPM");
					lcd_draw_string(88, 4, temp);
				
				
					lcd_draw_string(80, 6, "100PPM");
					break;
					
				case 2:
					if(language == Chinese)
					{
						lcd_show_string(48, 0, "氧气");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(48, 0, "O2");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
				
					
				
					sprintf(temp, "%.1f", o2_LAlarm.x);
					lcd_draw_char(120, 2, '%');
					lcd_draw_string(88, 2, temp);
					
					sprintf(temp, "%.1f", o2_HAlarm.x);		
					lcd_draw_char(120, 4, '%');
					lcd_draw_string(88, 4, temp);
				
				
					
					lcd_draw_string(88, 6, "30.0%");
					break;
			
				case 3:
					if(language == Chinese)
					{
						lcd_show_string(32, 0, "一氧化碳");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(32, 0, "C0");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
					
					
					temp[0] = '0' + co_LAlarm / 10;
					temp[1] = '0' + co_LAlarm %10;
					temp[2] = '\0';
					
					
					lcd_draw_string(104, 2, "PPM");
					lcd_draw_string(88, 2, temp);
					
				  temp[0] = '0' + co_HAlarm / 100;
					temp[1] = '0' + co_HAlarm % 100/10;
					temp[2] = '0' + co_HAlarm %10;
					temp[3] = '\0';				
					
					lcd_draw_string(104, 4, "PPM");
					lcd_draw_string(80, 4, temp);
				
				
					
					lcd_draw_string(72, 6, "1000PPM");
					break;
				
				case 4:
					if(language == Chinese)
					{
						lcd_show_string(32, 0, "可燃气体");
						lcd_show_string(0, 2, "低报");
						lcd_show_string(0, 4, "高报");
						lcd_show_string(0, 6, "量程");
					}
					else if(language == English)
					{
						lcd_draw_string(32, 0, "EX");
						lcd_draw_string(0, 2, "Alarm L");
						lcd_draw_string(0, 4, "Alarm H");
						lcd_draw_string(0, 6, "Alarm R");
					}
					
					
					temp[0] = '0' + fire_LAlarm / 10;
					temp[1] = '0' + fire_LAlarm %10;
					temp[2] = '\0';
					
					
					lcd_draw_string(96, 2, "%LEL");
					lcd_draw_string(80, 2, temp);
					
					temp[0] = '0' + fire_HAlarm /10;
					temp[1] = '0' + fire_HAlarm %10;
					temp[2] = '\0';				
					
					lcd_draw_string(96, 4,"%LEL");
					lcd_draw_string(80, 4, temp);
				
				
					
					lcd_draw_string(72, 6, "100%LEL");
					break;
			}
			break;
		
		default:
			break;
	}
}











/*******************************
*	函数功能：系统的日常工作流程
*			
*			描述: 将系统工作划分为 
*														 初始化状态，
*														 监控状态，
*														 密码输入状态，
*														 菜单设置状态,
********************************/
void System_Work(void)
{
	static uint8_t chg_sta = 0;
	static uint8_t pp_count = 0;
	
	switch(SystemWorkSt)
	{
		case IDLE:
			break;
		
		
		case SYSTEM_SEL_TEST:
				self_check();
				
			break;
		
		
		case NORMAL_WORK_PAGE:
				Update_DateAndTime();
				//界面显示
				if(chg_sta == 0)
					monitor_interface();
				else
					information_interface();
				
				//按键处理
				if(KeyValue == 3 && F_NewKey == True)
				{
					F_NewKey = False;
					glcd_clear_now();
					chg_sta = chg_sta?0:1;
				}
				else if(KeyValue == 2 && chg_sta == 0 && F_NewKey == True)
				{
					F_NewKey = False;
					
					PassFlag = 0;
					SystemWorkSt = PASSWORD_PAGE;
					glcd_clear_now();
					memset(PassTemp, 0, sizeof(PassTemp));
					passwork_interface();
				}
				else if(KeyValue == LONG_PRESS_KEY1 && F_NewKey == True)
				{	
					F_NewKey = False;
					
					glcd_clear_now();
					SystemWorkSt = POWER_STATE;
					fflag = 0;
				}
				
				
				
				if(backlight_time > 0)
						GLCD_BACKLIGHT(1);
				else if(backlight_arg == 0)
						GLCD_BACKLIGHT(0);
			break;
				
		case PASSWORD_PAGE:	//密码输入界面
			//passwork_interface();
			Handle_PassWork();
			if(backlight_time <= 0)
			{
				SystemWorkSt = NORMAL_WORK_PAGE;
				SubMenuWorkSt = SUB_MENU_IDLE;
				glcd_clear_now();
			}
			break;
		
		case MENU_SET:
			menu_interface();
			if(backlight_time <= 0)
			{
				SystemWorkSt = NORMAL_WORK_PAGE;
				SubMenuWorkSt = SUB_MENU_IDLE;
				glcd_clear_now();
			}
			break;
		
		case 	POWER_STATE:

			if(tnum <= 0 && pp_count <= 3)
			{
					pp_count += 1;
					tnum = 80;
					LED_1 = ~LED_1;
					if(fflag == 0)
					{
						Shake_Pin(1);
						fflag = 1;
					}
					else 
					{
						Shake_Pin(0);
						fflag = 0;
					}
			}
			
			lcd_show_string(48,2,"关机");
			lcd_show_chinese(0, 6, "是");
			lcd_show_chinese(112, 6, "否");
			lcd_draw_char(80, 2, '?');
			
			if(KeyValue == 3 && F_NewKey == True)
			{
				POWERDOWN = 0;
			}
			else if(KeyValue == 2 && F_NewKey == True)
			{
				F_NewKey = False;
				SystemWorkSt = NORMAL_WORK_PAGE;
				SubMenuWorkSt = SUB_MENU_IDLE;
				glcd_clear_now();
				pp_count = 0;
			}
			if(backlight_time <= 0)
			{
				SystemWorkSt = NORMAL_WORK_PAGE;
				SubMenuWorkSt = SUB_MENU_IDLE;
				glcd_clear_now();
				pp_count = 0;
			}
			
			break;
		
		default:
				break;
	}
}







int main(void)
{
	uint16_t adc_value = 0;
	uint8_t interface_staue = 0;
	

	

  lcd_initialize();
  Led_Init();
	Key_Init();
	Timer_init();
	Usart_Init(9600);
	Ds1302_Init();
	Set1302();
	YX3P_Init();
	InitADC();
	Shake_Init();
	System_Init();
	

	
	SystemWorkSt = SYSTEM_SEL_TEST;
	SubMenuWorkSt = SUB_MENU_IDLE;
	Selfcheck_System = ON_IDLE;
	

	
	
	while(1)
	{
		//key_scan();
			key_driver();
		//SendData(KeyValue);
		
		if(tcount <= 0)
		{
			Transform_AdcGas();
			tcount = 1000;

		//	circulation_test();
		}
		System_Work();
		if(SystemWorkSt != SYSTEM_SEL_TEST && SystemWorkSt != POWER_STATE)
		{
				Do_Alarm();
		}
	}
	return 0;
}




