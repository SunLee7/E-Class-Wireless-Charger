#include <stdio.h>
#include <STC8.H>

void UartSend(char dat)
{
	SBUF = dat;	        //SUBF����/���ͻ�����(�ֽд���ͨ�����⹦�ܼĴ���)
	while(!TI);				// �������ݴ���	(TI�����жϱ�־)
	TI = 0;
}

char putchar(char c)
{
	UartSend(c);
	return c;
}