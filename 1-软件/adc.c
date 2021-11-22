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
	P1M0= 0x00;                                //����P1ΪADC��
	P1M1|= 0x3C;
	ADCCFG=0X0f;	 //ת���ٶ�����  Ĭ�����F
}

uint16 ReadADC(uint8 ch)
{   
		uint16 adc,ADC;
                                     //ѡ��P1�ڵ���һ�� ����Ŀں�chҪ��Ӧ���ܴﵽѡ��ÿ�
    ADC_CONTR = ADC_POWER | ADC_START | ch ;//����adc��Դ����ʼA/Dת�� �����òɼ���
                                                         //��ô��������Ҫԭ����ǲ���λѰַ
                                                         //ͨ��ѡ���ں�3λ����ֱ����һ��������ʾch
                                                         //����ch=6 ��ô��Ӧ�ĺ���λ����110           
    _nop_();         //Must wait before inquiry �� 
    _nop_(); //����ADC_CONTR�Ĵ��������2��CPUʱ�����ڵ���ʱ�����ܱ�ֵ֤��д��ADC_CONTR�Ĵ���
    while (!(ADC_CONTR & ADC_FLAG));//Wait complete flag 		�ȴ�ת�����
		ADC_CONTR &= ~ADC_FLAG;         //Close ADC ����־λ����ȴ��´�Ӳ����1

		//12λadc   ��ȡ����
		adc=ADC_RES;
		ADC_RESL=ADC_RESL&0XF0;
		adc=(adc<<4)|(ADC_RESL>>4);
    return adc;
}