
#include "adc.h"


uint16_t fire_gas_adc;
uint16_t co_gas_adc;
uint16_t o2_gas_adc;
uint16_t hydrothion_gas_adc;



/*----------------------------
Get ADC result
----------------------------*/
uint16_t GetADCResult_Normal(uint8_t ch)
{
	uint16_t temp = 0;
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
   	Delay(1);
    while (!(ADC_CONTR & ADC_FLAG));//Wait complete flag
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

	temp = ADC_RES << 2;
	temp |= ADC_LOW2;
    return (temp);                 //Return ADC result
}




/*----------------------------
Get ADC result
----------------------------*/
uint16_t GetADCResult(uint8_t ch)
{ 
    unsigned int Tad=0,MinAd=0x0fff,MaxAd=0x0000,TempAdd=0;
    unsigned char t=0;

    for(t=0;t<10;t++)
    {
		Tad = GetADCResult_Normal(ch); //È¡µÃÒ»´Î×ª»»Öµ
		if (Tad>MaxAd)
		{
		MaxAd=Tad ;//»ñµÃµ±Ç°µÄ×î´óÖµ
		}
		if (Tad<MinAd)
		{
		MinAd=Tad; //»ñµÃµ±Ç°µÄ×îĞ¡Öµ
		}
		TempAdd+=Tad; //×ª»»ÖµÀÛ¼Ó
	}

	TempAdd-=MinAd; //È¥µô×îĞ¡Öµ
	TempAdd-=MaxAd; //È¥µô×î´óÖµ
	TempAdd>>=3; //ÇóÆ½¾ùÖµ
	return(Tad);
}




/*----------------------------
Initial ADC sfr
----------------------------*/
void InitADC(void)
{
    P1ASF = 0x0f ;                   //Open 4 channels ADC function
    ADC_RES = 0;                    //Clear previous result
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    Delay(2);                       //ADC power-on and delay
}



/**************************
* å‡½æ•°åŠŸèƒ½: è·å–4è·¯ADCçš„é‡‡æ ·å€¼
*			å‚æ•°ï¼šæ— 
*			è¿”å›å€¼ï¼šæ— 
*
*	è¯´æ˜ ï¼šæŠŠé‡‡æ ·åˆ°çš„å€¼æ”¾åˆ° 4ä¸ªå…¨å±€å˜é‡ä¸­
***************************/

void collect_adc_value(void)
{
	
	 fire_gas_adc= GetADCResult(0);
	 o2_gas_adc = GetADCResult(1);
	 co_gas_adc = GetADCResult(2);
	 hydrothion_gas_adc= GetADCResult(3);
}




