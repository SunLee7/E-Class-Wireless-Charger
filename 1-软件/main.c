#include <STC8.H>
#include <stdio.h>
#include "oled.h"
#include "debug.h"
#include "adc.h"
#include "intrins.h"

char str[100];
uint8 times=0;
uint16 V=0,I=0;

void Delay1000ms()		//@24.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 92;
	j = 50;
	k = 238;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Timer0Init(void)		//5����@24.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xF0;		//���ö�ʱ��ֵ
	TH0 = 0xD8;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

void UartInit(void)		//115200bps@24.000MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xCC;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	ES = 1;
}

void PWMInit()	//��ǿ��pwm��ʼ��
{
	P_SW2 = 0x80;
	PWMCKS = 0x00; // PWMʱ��Ϊϵͳʱ��
	PWMC = (24000000/320000); //����PWM����(320000Hz)
	
	PWM2T1= 0;
	PWM2T2= 0;
	PWM2CR= 0x80; //ʹ��PWM4���
	
	PWM3T1= 0;
	PWM3T2= 0;
	PWM3CR= 0x80; //ʹ��PWM5���
	
	P_SW2 = 0x00;
	PWMCR = 0x80; //����PWMģ��
}

void main()
{
	Timer0Init();
	PWMInit();
//	UartInit();
	ADCInit();
	OLED_Init();
	OLED_Clear();
	
	Delay1000ms();
	
	P_SW2 = 0x80;
	PWM2T1= 38;
	PWM2T2= 19;
	PWM3T1= 0;
	PWM3T2= 57;
	P_SW2 = 0x00;
	
	ET0=1;
	EA=1;
	
	while(1)
	{
		
	}
}

//5ms�ж�һ��
void TM0_Isr() interrupt 1
{
	times++;
	//100ms��Ļˢ��һ��
	if(times==20)
	{
		times=0;
//		V = 1064;
		sprintf(str,"Voltage:%.2fV   ",V*0.0134);
		OLED_ShowString(0,0,str);
		sprintf(str,"Current:%.2fA   ",I*0.00244);
		OLED_ShowString(0,2,str);
		sprintf(str,"Power:%.2fW     ",I*0.00244*V*0.0134);
		OLED_ShowString(0,4,str);
		V=0;I=0;
	}
	V+=0.05*ReadADC(3);
	I+=0.05*ReadADC(4);
}

//��λ�������жϣ����û����
void uart_int(void) interrupt 4 using 1 //�����ж�
{ 
	unsigned char tmp;
	if (RI) 
	{
    tmp = SBUF;//��ȡ�ֽ�����
		my_debug(tmp);//��λ�����պ���
		RI = 0;
  }
}