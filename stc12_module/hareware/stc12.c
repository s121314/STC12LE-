
#include "stc12.h"
#include "font.h"
#include "usart.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t zi_club[] = 
"正在启动硫化氢氧气一碳可燃电量已监测日期时间密码验证增加\
减少确认菜单报警记录低设置高零点标定背光通道信息语言切换退\
出向上下选项返回删除最后条是否保存值数据异常请稍候择自常亮程体中英文页该全部无\
探器开机检振动灯关";


uint8_t c_club[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWZXYabcdefghijklmnopqrstuvwxzy%.:>? ";


//uint8_t fram_buffer[128*64/8] ;

//延时变量

uint16_t tcount = 0;
uint16_t tnum = 0;
uint16_t shake_num = 0;
uint16_t sound_delaay_num = 0;
uint16_t pop_up_time = 0;
uint16_t another_pop_time = 0;
uint16_t backlight_time = 0;
uint16_t begin_time = 0;
uint16_t ks_X1ms = 0;



void Delay(int n)
{
    int i,j;

    for (i=0; i<n; i++)
    for (j=0; j<1;j++);
}


/************************************************************
*	LCD 电池图标
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						len			(进度条的长度)
* 返回值： 无 
*************************************************************/

void lcd_draw_battery(uint8_t column, uint8_t page)
{
  	uint8_t count, msb, lsb;
 
    lsb = ((column)&(0x0F));
    msb = ((column>>4)&(0x0F));
    msb |= 0x10;
    page |= 0xB0;
    
    glcd_command(msb);
    glcd_command(lsb);
    glcd_command(page);
	
		for (count=0;count<(4*16);count++)//each character is 5px wide - 'count'
    {
			glcd_data(Battery[count]);
			if(count == 31)
			{
				glcd_command(msb);
				glcd_command(lsb);
				glcd_command(page+1);
			}
			
    }
}








/************************************************************
*	LCD 显示进度条
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						len			(进度条的长度)
* 返回值： 无 
*************************************************************/

void lcd_draw_bar(uint8_t column, uint8_t page,uint8_t len)
{
  	uint8_t count, msb, lsb;
 
    lsb = ((column)&(0x0F));
    msb = ((column>>4)&(0x0F));
    msb |= 0x10;
    page |= 0xB0;
    
    glcd_command(msb);
    glcd_command(lsb);
    glcd_command(page);
	
		for (count=0;count<len;count++)//each character is 5px wide - 'count'
    {
			if(count == 0 || count == (len - 1))
			{
				glcd_data(0xff);
			}
			else 
			{
					glcd_data(0x81);   
			}
    }
}


/************************************************************
*	LCD 显示进度条滑动，对column 进行加加,在一个for循环里面
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
* 返回值： 无 
*************************************************************/
void bar_slider(uint8_t column, uint8_t page)
{
	  uint8_t  msb, lsb;
 
    lsb = ((column)&(0x0F));
    msb = ((column>>4)&(0x0F));
    msb |= 0x10;
    page |= 0xB0;
    
    glcd_command(msb);
    glcd_command(lsb);
    glcd_command(page);
		
		glcd_data(0xff);
}





/************************************************************
*	LCD 显示一个字符函数
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						letter	(要打印的汉字)
* 返回值： 无 
*************************************************************/

void lcd_show_chinese(uint8_t column, uint8_t page, uint8_t *letter)
{
  	uint8_t count, msb, lsb;
		uint8_t n = 0;
 
    lsb = ((column)&(0x0F));
    msb = ((column>>4)&(0x0F));
    msb |= 0x10;
    page |= 0xB0;
    
    glcd_command(msb);
    glcd_command(lsb);
    glcd_command(page);

	while(1)
	{
		if((*letter == zi_club[3*n]) && (*(letter+1) == zi_club[3*n + 1]) && (*(letter + 2) == zi_club[3*n + 2]))
		{
			break;
		}
		n++;
	}
	
	//SendData(n);		//打印调试用
	
	for (count=0;count<32;count++)//each character is 5px wide - 'count'
    {
			glcd_data(*(Chin + count + n*32));   
			if(count == 15)
			{
				glcd_command(msb);
    		glcd_command(lsb);
				glcd_command(page+1);	
			}	
    }
}



/************************************************************
*	LCD 显示一串汉字符函数
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为8页,每页占8个像素点)
*						string	(要打印的汉字字符串)
* 返回值： 无 
*************************************************************/
void lcd_show_string(uint8_t column, uint8_t page, uint8_t *string)
{
    uint8_t i = 0;
		uint8_t c_temp = column;
	
    while(*string != 0)
		{
       lcd_show_chinese(c_temp, page, string);
       string += 3;
       i++;
			 c_temp	= column+(16*i);
			 if(c_temp+16 > GLCD_LCD_WIDTH)
			 {
					page += 2;
			 }
    }	
}





/************************************************************
*	LCD 显示一个ACLII字符函数
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						letter	(要打印的ASCII字符)
* 返回值： 无 
*************************************************************/


void lcd_draw_char(uint8_t column, uint8_t page, uint8_t letter)
{
  	uint8_t count, msb, lsb;
		uint8_t n = 0;
 
    lsb = ((column)&(0x0F));
    msb = ((column>>4)&(0x0F));
    msb |= 0x10;
    page |= 0xB0;
    
    glcd_command(msb);
    glcd_command(lsb);
    glcd_command(page);

	while(1)
	{
		if(letter == c_club[n]) //&& (*(letter+1) == zi_club[3*n + 1]) && (*(letter + 2) == zi_club[3*n + 2]))
		{
			break;
		}
		n++;
	}
	
//	n = sizeof(c_club);
//	SendData(n);		//打印调试用
	
	for (count=0;count<16;count++)//each character is 5px wide - 'count'
    {
			glcd_data(*(Ascii_1 + n*16 + count));   
			if(count == 7)
			{
				glcd_command(msb);
    		glcd_command(lsb);
				glcd_command(page+1);	
			}	
    }
}







/************************************************************
*	LCD 显示一串ASCII字符函数
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						string	(要打印的ASCII字符串)
* 返回值： 无 
*************************************************************/


void lcd_draw_string(uint8_t column, uint8_t page, uint8_t *string){
    uint8_t i = 0;
    uint8_t c_temp = column;
		while(*string != '\0')
		{
       lcd_draw_char(c_temp, page, *string);
       string += 1;
       i++;
			 c_temp	= column+(8*i);
			 if(c_temp+8 > GLCD_LCD_WIDTH)
			 {
					page += 2;
			 }
    }	
}






/************************************************************
*	LCD 显示一个ACLII字符函数
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						letter	(要打印的ASCII字符)
* 返回值： 无 
*************************************************************/


void lcd_shadow_draw_char(uint8_t column, uint8_t page, uint8_t letter)
{
  	uint8_t count, msb, lsb;
		uint8_t n = 0;
 
    lsb = ((column)&(0x0F));
    msb = ((column>>4)&(0x0F));
    msb |= 0x10;
    page |= 0xB0;
    
    glcd_command(msb);
    glcd_command(lsb);
    glcd_command(page);

	while(1)
	{
		if(letter == c_club[n]) //&& (*(letter+1) == zi_club[3*n + 1]) && (*(letter + 2) == zi_club[3*n + 2]))
		{
			break;
		}
		n++;
	}
	
//	n = sizeof(c_club);
//	SendData(n);		//打印调试用
	
	for (count=0;count<16;count++)//each character is 5px wide - 'count'
    {
			glcd_data(~(*(Ascii_1 + n*16 + count)));   
			if(count == 7)
			{
				glcd_command(msb);
    		glcd_command(lsb);
				glcd_command(page+1);	
			}	
    }
}


/************************************************************
*	LCD 显示一串ASCII字符函数 (阳码显示)
*	传入参数：column （列地址）
*						page		(页数，该屏把行划分为6页,每页占8个像素点)
*						string	(要打印的ASCII字符串)
* 返回值： 无 
*************************************************************/


void lcd_shadow_draw_string(uint8_t column, uint8_t page, uint8_t *string){
    uint8_t i = 0;
    uint8_t c_temp = column;
		while(*string != '\0')
		{
       lcd_shadow_draw_char(c_temp, page, *string);
       string += 1;
       i++;
			 c_temp	= column+(8*i);
			 if(c_temp+8 > GLCD_LCD_WIDTH)
			 {
					page += 2;
			 }
    }	
}




void glcd_power_up(void)
{
	glcd_command(0xa4); // Display all points OFF
	glcd_command(0xad);	// Static indicator ON
	glcd_command(0x00);	// Static indicator register, not Blinking
	glcd_command(0xaf);

	return;
}

void glcd_set_column_upper(uint8_t addr)
{
	glcd_command(ST7565R_COLUMN_ADDRESS_SET_UPPER | (addr >> 4));
}

void glcd_set_column_lower(uint8_t addr)
{
	glcd_command(ST7565R_COLUMN_ADDRESS_SET_LOWER | (0x0f & addr));
}


void glcd_set_y_address(uint8_t y)
{
	glcd_command(ST7565R_PAGE_ADDRESS_SET | (0x0F & y)); /* 0x0F = 0b00001111 */
}

void glcd_set_x_address(uint8_t x)
{
	glcd_set_column_upper(x);
	glcd_set_column_lower(x);	
}

 void glcd_power_down(void)
{
	/* Command sequence as in ST7565 datasheet */
	glcd_command(0xac);	// Static indicator off
	glcd_command(0x00);	// Static indicator register, not blinking
	glcd_command(0xae); // Display OFF
	glcd_command(0xa5); // Display all points ON

	/* Display is now in sleep mode */
}

/** Clear the display immediately, does not buffer */
void glcd_clear_now(void)
{
	uint8_t page;
	uint8_t col;
	for (page = 0; page < GLCD_NUMBER_OF_BANKS; page++) {
		glcd_set_y_address(page);
		glcd_set_x_address(0);
		for (col = 0; col < GLCD_NUMBER_OF_COLS; col++) {
			glcd_data(0);
		}			
	}
}
/*******************************************
*函数功能: 擦除一片区域
*	
*	传入的参数: start_page, start_colum, end_page, end_colum
*							(开始页，   开始列，      结束页,			结束列)
********************************************/

void glcd_clear(uint8_t start_page, uint8_t start_colum, uint8_t end_page, uint8_t end_colum)
{
		uint8_t page;
		uint8_t col;
		for (page = start_page; page <= end_page; page++) {
			glcd_set_y_address(page);
			glcd_set_x_address(start_colum);
			for (col = start_colum; col < end_colum; col++) {
				glcd_data(0);
			}			
		}
}



 //
void LcdSPI_Init(void)
{
	P0M1 &= ~0x1f;
	P0M0 |= 0x1f;
	
	P4M0 |= (1 << 2);
	P4M1 &= ~(1 << 2); 

	GLCD_DESELECT();
	GLCD_A0_HIGH();
	LCD_SCL_HIGHT();
	LCD_SI_HIGHT();
	GLCD_RESET_HIGH();
}



void glcd_spi_write(uint8_t dat)
{
	uint8_t  i;
	GLCD_SELECT();

	for(i = 0; i < 8; i++)
	{
		LCD_SCL_LOW();
		if(dat & 0x80)
		{
			LCD_SI_HIGHT();
		}
		else
		{
			LCD_SI_LOW();
		}
	//	Delay(1);
		LCD_SCL_HIGHT();
	//	Delay(1);
		dat = dat << 1;
	}

	LCD_SCL_HIGHT();
	GLCD_DESELECT();
}


void glcd_command(uint8_t c)
{
	GLCD_A0_LOW();
	glcd_spi_write(c);	
}


void glcd_data(uint8_t c)
{
	GLCD_A0_HIGH();
	glcd_spi_write(c);	
}


void glcd_set_contrast(uint8_t val) {
	/* Can set a 6-bit value (0 to 63)  */

	/* Must send this command byte before setting the contrast */
	glcd_command(0x81);
	
	/* Set the contrat value ("electronic volumne register") */
	if (val > 63) {
		glcd_command(63);
	} else {
		glcd_command(val);
	}
	return;
}



void glcd_reset(void)
{
	/* Toggle RST low to reset. Minimum pulse 100ns on datasheet. */
	GLCD_SELECT();
	GLCD_RESET_LOW();

	Delay(5);
	GLCD_RESET_HIGH();
	GLCD_DESELECT();
}


void glcd_all_on(void)
{
	glcd_command(ST7565R_DISPLAY_ALL_ON);
}

void glcd_normal(void)
{
	glcd_command(ST7565R_DISPLAY_NORMAL);
}

void glcd_set_start_line(uint8_t addr)
{
	glcd_command( ST7565R_SET_START_LINE | (0x3F & addr)); /* 0x3F == 0b00111111 */
}


void lcd_initialize(void)
{
 	
	LcdSPI_Init();
	glcd_reset();

	//backlight	need//
	GLCD_BACKLIGHT(1);

//    glcd_command(0xA2);                             // added 1/9 bias
//    glcd_command(0xA0);                             // ADC segment driver direction (A0 = normal)
//    glcd_command(0xC8);                             // COM output scan direction (C0 = normal/C8)
//    glcd_command(0x25);                             // resistor ratio
//    glcd_command(0x81);                             // electronic volume mode set
//    glcd_command(0x15);                             // electronic volume register set
//    glcd_command(0x2F);                             // operating mode
//    glcd_command(0x40);                             // start line set
//    glcd_command(0xAF);                             // display on
	
	
		/* Default init sequence */
	/* Currently just set the same as GLCD_INIT_NHD_C12864A1Z_FSW_FBW_HTT */

	glcd_command(0xa0); /* ADC select in normal mode */
	glcd_command(0xae); /* Display OFF */
	glcd_command(0xc8); /* Common output mode select: reverse direction (last 3 bits are ignored) */
	glcd_command(0xa2); /* LCD bias set at 1/9 */
	glcd_command(0x2f); /* Power control set to operating mode: 7 */
	glcd_command(0x26); /* Internal resistor ratio, set to: 6 */
	glcd_set_contrast(10); /* Set contrast, value experimentally determined, value 0 to 63 */
	glcd_command(0xaf); /* Display on */


	glcd_all_on();
	Delay(10);
	glcd_normal();
	glcd_set_start_line(0);
	glcd_clear_now();

}



/********LED configure*******
*P2.4 --- LED1
*
****************************/


void Led_Init(void)
{
	P2M1 &= ~(1 << 4);
	P2M0 |= 1 << 4;
	
	LED_1 = 0;		
}

/********Timer0 configure***
* 12T mode, 1ms interrupt
*
****************************/

void Timer_init(void)
{
	TMOD = 0x01; // mode 1
	TH0 = T0MS >> 8;
	TL0 = T0MS;		// delay 1ms
	TR0 = 1;		//timer0 start running
	ET0 = 1;		//enable timer0 interrupt
	EA = 1;			//open global interrupt switch	
}


/**Timer0 interrupt routine***
*
*
******************************/
void tm0_isr() interrupt 1 using 1
{
	TH0 = T0MS >> 8;
	TL0 = T0MS;		// delay 1ms
	
	if(tcount > 0)
		tcount--;
	if(tnum > 0)
		tnum--;
	if(shake_num > 0)
		shake_num--;
	if(sound_delaay_num > 0)
		sound_delaay_num--;
	if(pop_up_time > 0)
		pop_up_time--;
	if(another_pop_time > 0)
		another_pop_time--;
	if(backlight_time > 0)
		backlight_time--;
	if(begin_time > 0)
		begin_time--;
	if(ks_X1ms > 0)
		ks_X1ms--;
}



