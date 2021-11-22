#include <stdio.h>
#include <STC8.H>

void UartSend(char dat)
{
	SBUF = dat;	        //SUBF接受/发送缓冲器(又叫串行通信特殊功能寄存器)
	while(!TI);				// 等特数据传送	(TI发送中断标志)
	TI = 0;
}

char putchar(char c)
{
	UartSend(c);
	return c;
}