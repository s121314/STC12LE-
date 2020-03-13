
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
		Tad = GetADCResult_Normal(ch); //ȡ��һ��ת��ֵ
		if (Tad>MaxAd)
		{
		MaxAd=Tad ;//��õ�ǰ�����ֵ
		}
		if (Tad<MinAd)
		{
		MinAd=Tad; //��õ�ǰ����Сֵ
		}
		TempAdd+=Tad; //ת��ֵ�ۼ�
	}

	TempAdd-=MinAd; //ȥ����Сֵ
	TempAdd-=MaxAd; //ȥ�����ֵ
	TempAdd>>=3; //��ƽ��ֵ
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
* 函数功能: 获取4路ADC的采样值
*			参数：无
*			返回值：无
*
*	说明 ：把采样到的值放到 4个全局变量中
***************************/

void collect_adc_value(void)
{
	
	 fire_gas_adc= GetADCResult(0);
	 o2_gas_adc = GetADCResult(1);
	 co_gas_adc = GetADCResult(2);
	 hydrothion_gas_adc= GetADCResult(3);
}




