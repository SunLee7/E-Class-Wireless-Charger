#include <STC8.H>
#include "intrins.h"
#include "debug.h"

uint16 VP13=0;
uint16 VP12=0;
uint16 VP15=0;
uint16 VP14=0;
uint16 VP00=0;
uint16 VP01=0;
uint16 VP02=0;

void ADCInit(void)
{
	P1M0= 0x00;                                //设置P1为ADC口
	P1M1|= 0x3C;
	ADCCFG=0X0f;	 //转化速度配置  默认最大F
}

uint16 ReadADC(uint8 ch)
{   
		uint16 adc,ADC;
                                     //选择P1口的哪一口 这里的口和ch要对应才能达到选择该口
    ADC_CONTR = ADC_POWER | ADC_START | ch ;//启动adc电源，开始A/D转换 ，配置采集口
                                                         //这么用语句的主要原因就是不能位寻址
                                                         //通道选择在后3位所以直接用一个整数表示ch
                                                         //例如ch=6 那么对应的后三位就是110           
    _nop_();         //Must wait before inquiry ， 
    _nop_(); //设置ADC_CONTR寄存器后需加2个CPU时钟周期的延时，才能保证值被写入ADC_CONTR寄存器
    while (!(ADC_CONTR & ADC_FLAG));//Wait complete flag 		等待转化完成
		ADC_CONTR &= ~ADC_FLAG;         //Close ADC 将标志位清零等待下次硬件置1

		//12位adc   读取方法
		adc=ADC_RES;
		ADC_RESL=ADC_RESL&0XF0;
		adc=(adc<<4)|(ADC_RESL>>4);
    return adc;
}