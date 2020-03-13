#ifndef _STC12_H
#define	_STC12_H

#include "reg51.h"


sfr P0M1 = 0x93;
sfr P0M0 = 0x94;

sfr P2M1 = 0x95;
sfr P2M0 = 0x96;

sfr P4M0 = 0xB4;	
sfr P4M1 = 0xB3;

sfr P4 = 0xC0;




/* SPIx Communication boards Interface 
*  P04 --> LCD_CS
*  P03 --> LCD_RES
*  P02 --> LCD_A0 (c/s)
*  P01 --> LCD_D6 (scl)
*  P00 --> LCD_D7 (SI)
*/

sbit P00 = P0^0;
sbit P01 = P0^1;
sbit P02 = P0^2;
sbit P03 = P0^3;
sbit P04 = P0^4;
//backlight
sbit P42 = P4^2;


//LED
sbit P24 = P2^4;







#define	  	  GLCD_SELECT()	 	        (P04 = 0)
#define 	  GLCD_DESELECT()		    (P04 = 1)

#define	      GLCD_A0_LOW()	 	        (P02 = 0)
#define 	  GLCD_A0_HIGH()		    (P02 = 1)

#define	      LCD_SCL_LOW()	 	        (P01 = 0)
#define 	  LCD_SCL_HIGHT()	 	    (P01 = 1)

#define	      LCD_SI_LOW()	 	        (P00 = 0)
#define 	  LCD_SI_HIGHT()		    (P00 = 1)

#define	      GLCD_RESET_LOW()	 	    (P03 = 0)
#define 	  GLCD_RESET_HIGH()		    (P03 = 1)

#define		  GLCD_BACKLIGHT(x)			(P42 = x)

#define		LED_1		P24	

#define	uint8_t		unsigned char 
#define	uint16_t    unsigned int

//timer0 setup
#define		FOSC	11059200
#define		T0MS	(65536 - FOSC/12/1000)



#define GLCD_LCD_WIDTH 128
#define GLCD_LCD_HEIGHT 64

//#define GLCD_NUMBER_OF_BANKS (GLCD_LCD_WIDTH / 8)

#define GLCD_NUMBER_OF_BANKS (GLCD_LCD_HEIGHT	/ 8)
#define GLCD_NUMBER_OF_COLS  GLCD_LCD_WIDTH

/* Commands */
#define ST7565R_DISPLAY_ON               0xAF /* 0b10101111 */
#define ST7565R_DISPLAY_OFF              0xAE /* 0b10101110 */
#define ST7565R_PAGE_ADDRESS_SET         0xB0 /* 0b10110000 */
#define ST7565R_COLUMN_ADDRESS_SET_LOWER 0x00 
#define ST7565R_COLUMN_ADDRESS_SET_UPPER 0x10 
#define ST7565R_DISPLAY_NORMAL           0xA4 /* 0b10100100 */
#define ST7565R_DISPLAY_ALL_ON           0xA5 /* 0b10100101 */
#define ST7565R_NORMAL                   0xA0 /* 0b10100000 */
#define ST7565R_REVERSE                  0xA1 /* 0b10100001 */
#define ST7565R_RESET                    0xE2 /* 0b11100010 */
#define ST7565R_SET_START_LINE           (1<<6)





void Delay(int n);
void lcd_initialize(void);
void glcd_data(uint8_t c);
void glcd_command(uint8_t c);
void lcd_draw_char(uint8_t column, uint8_t page, uint8_t letter);
void lcd_draw_string(uint8_t column, uint8_t page, char *string);
void Led_Init(void);
void Timer_init(void);
void glcd_power_up(void);
void glcd_power_down(void);
void lcd_show_chinese(uint8_t column, uint8_t page, uint8_t *letter);
void lcd_show_string(uint8_t column, uint8_t page, uint8_t *string);
void glcd_clear_now(void);
void glcd_clear(uint8_t start_page, uint8_t start_colum, uint8_t end_page, uint8_t end_colum);
void lcd_shadow_draw_string(uint8_t column, uint8_t page, uint8_t *string);
void lcd_shadow_draw_char(uint8_t column, uint8_t page, uint8_t letter);
void lcd_draw_bar(uint8_t column, uint8_t page,uint8_t len);
void bar_slider(uint8_t column, uint8_t page);
void lcd_draw_battery(uint8_t column, uint8_t page);

#endif