
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
		Tad = GetADCResult_Normal(ch); //取得一次转换值
		if (Tad>MaxAd)
		{
		MaxAd=Tad ;//获得当前的最大值
		}
		if (Tad<MinAd)
		{
		MinAd=Tad; //获得当前的最小值
		}
		TempAdd+=Tad; //转换值累加
	}

	TempAdd-=MinAd; //去掉最小值
	TempAdd-=MaxAd; //去掉最大值
	TempAdd>>=3; //求平均值
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
* 鍑芥暟鍔熻兘: 鑾峰彇4璺疉DC鐨勯噰鏍峰��
*			鍙傛暟锛氭棤
*			杩斿洖鍊硷細鏃�
*
*	璇存槑 锛氭妸閲囨牱鍒扮殑鍊兼斁鍒� 4涓叏灞�鍙橀噺涓�
***************************/

void collect_adc_value(void)
{
	
	 fire_gas_adc= GetADCResult(0);
	 o2_gas_adc = GetADCResult(1);
	 co_gas_adc = GetADCResult(2);
	 hydrothion_gas_adc= GetADCResult(3);
}




