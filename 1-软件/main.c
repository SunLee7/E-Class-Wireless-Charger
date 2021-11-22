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

void Timer0Init(void)		//5毫秒@24.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xF0;		//设置定时初值
	TH0 = 0xD8;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void UartInit(void)		//115200bps@24.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xCC;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES = 1;
}

void PWMInit()	//增强型pwm初始化
{
	P_SW2 = 0x80;
	PWMCKS = 0x00; // PWM时钟为系统时钟
	PWMC = (24000000/320000); //设置PWM周期(320000Hz)
	
	PWM2T1= 0;
	PWM2T2= 0;
	PWM2CR= 0x80; //使能PWM4输出
	
	PWM3T1= 0;
	PWM3T2= 0;
	PWM3CR= 0x80; //使能PWM5输出
	
	P_SW2 = 0x00;
	PWMCR = 0x80; //启动PWM模块
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

//5ms中断一次
void TM0_Isr() interrupt 1
{
	times++;
	//100ms屏幕刷新一次
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

//上位机接收中断，这次没用上
void uart_int(void) interrupt 4 using 1 //串口中断
{ 
	unsigned char tmp;
	if (RI) 
	{
    tmp = SBUF;//获取字节数据
		my_debug(tmp);//上位机接收函数
		RI = 0;
  }
}