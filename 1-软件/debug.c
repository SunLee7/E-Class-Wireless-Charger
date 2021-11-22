#include <STC8.H>
#include <string.h>
#include "debug.h"
#include "adc.h"
#include "myprintf.h"
#define RXBUFFERSIZE 1

int *BGV; //内部Bandgap电压值存放在idata中
//idata的EFH地址存放高字节
//idata的F0H地址存放低字节
//电压单位为毫伏(mV)

extern code uint16 display0[];
extern code uint16 display1[];
extern code uint16 display2[];
extern code uint16 display3[];


float Variable[20];     //变量
float Parameter[20];    //参数

uint8 SendPara,send_data_contorl=0,Save_Flag=0;

extern int MYIAP;

char RxBuffer[RXBUFFERSIZE];
uint8 aRxBuffer;
uint8 Uart1_Rx_Cnt = 0;


void my_putchar(uint8 temp)
{
		UartSend(temp);
}
/*用来通知上位机新的一组数据开始，要保存数据必须发送它*/
void Send_Begin()
{ 
  my_putchar(0x55);
  my_putchar(0xaa);
  my_putchar(0xae);
}

//测试数据生产 1
void Testdata_generate()    //generate生成
{
	BGV = (int idata *)0xef;
	Variable[0]=0;
	Variable[1]=0;
}

//发送变量
// Variable 变量 1
void Send_Variable()
{
  uint8 i=0,ch=0;
  float temp=0;
  uint8 Variable_num=16;
  my_putchar(0x55);
  my_putchar(0xaa);
  my_putchar(0xad);
  my_putchar(Variable_num);
  for(i=0;i<Variable_num;i++)
  {
    temp=Variable[i];
    ch=BYTE0(temp);
    my_putchar(ch);
    ch=BYTE1(temp);
    my_putchar(ch);
    ch=BYTE2(temp);
    my_putchar(ch);
    ch=BYTE3(temp);
    my_putchar(ch);
  }
  my_putchar(0x0d);
}

//修改 参数
void Modify_Parameter(uint8 *buff)     //Modify 修改 Parameter参数
{
  uint8 i=0,addr=0;
  float temp,example=123.1;
  uint8 Parameter_num=14; //14个可改参数
  /*          修改参数数组         */
  for(i=0;i<Parameter_num;i++)
  {
    BYTE0(temp)=*(uint8*)(buff+addr);
    addr++;
    BYTE1(temp)=*(uint8*)(buff+addr);
    addr++;
    BYTE2(temp)=*(uint8*)(buff+addr);
    addr++;
    BYTE3(temp)=*(uint8*)(buff+addr);
    addr++;
    Parameter[i]=temp;
  }
	Save_Parameter();
}
/*         从参数数组中更新参数值        */
void Save_Parameter()
{
	P43=Parameter[0];
}

//发送参数
void Send_Parameter()
{
  uint8 i=0,ch=0;
  float temp=0;
  uint8 Parameter_num=14;  //14个可改参数
  
  /*         将参数值更新到参数数组中  示例         */
 // Parameter[0]=i1;
	Parameter[1]=P20;
	Parameter[2]=P21;
	Parameter[3]=P23;
	Parameter[4]=P24;
  
  my_putchar(0x55);
  my_putchar(0xaa);
  my_putchar(0xab);
  my_putchar(Parameter_num);
  for(i=0;i<Parameter_num;i++)
  {
    temp=Parameter[i];
    ch=BYTE0(temp);
    my_putchar(ch);
    ch=BYTE1(temp);
    my_putchar(ch);
    ch=BYTE2(temp);
    my_putchar(ch);
    ch=BYTE3(temp);
    my_putchar(ch);
  }
  my_putchar(0X0b);//帧尾
}

void send_init()//上位机发数据初始化
{
  if(SendPara)
  {
     SendPara=0;
     Send_Parameter();
  }
  Send_Begin();
  Testdata_generate();
  Send_Variable();
}

void my_debug(unsigned char tmp)
{
	static uint8 recv;
  static uint8 data_cnt=0;
  static uint8 predata[10];
  static uint8 Recv_Buff[100];
  static uint8 Data_Receiving=0;   //Data_Receiving数据接收
  
  //if(uart_query(uart1)==1)  uart_getchar (uart1,(uint8*)(&recv));  //根据实际的串口来修改     uart_query查询是否接收到一个字节     uart_getchar等待接收 1 个字节
  /**********代表正在接收来自上位机的参数数据*********/
	recv = tmp;
	
  if(Data_Receiving)
  {
    if(data_cnt<56)
    {
      Recv_Buff[data_cnt]= recv;
      data_cnt++;
    }
    else
    {
      data_cnt=0;    //达到帧长
      Data_Receiving=0;
      if(recv==0xab)  //帧尾
      {
        Modify_Parameter(Recv_Buff);
        SendPara=1;      //参数回传，确认参数修改完成
				Save_Flag=1;
      }
    }
  }
  else
  {
    // beep=1; //开启蜂鸣器
    
    switch(recv)         //判断功能字
    {
    case 0x30:           //车模启停开关 收到数据后可以控制小车的启动和停
			
      break;
      
    case 0x31:           //数据发送开关
			
      IAP_CONTR = 0x60;//单片机准备开始isp下载
		
      /*********由于数据发送时要占用部分CPU资源和运行时间，小车运行时不建议发送数据，所以可以关闭发送*********/
      break;
      
    case 0x32:           //读取参数
      if(SendPara==0)
        SendPara=1;
      break;        
    case 0X33:             //修改参数
      Data_Receiving=1;
      data_cnt=0;
      break;
    case 0X34:             //保存参数
      break;
      
    case 0X35:             //SD卡数据保存开关
      
      break;
      
    case 0X36:             //读取SD卡数据保存起始扇区的信息
      
      break;
      
    case 0X37:             //SD卡上一帧
      
      break;
      
    case 0X38:             //SD卡下一帧
      
      break;
      
    case 0X39:             //扇区跳转
      
      break;
      
      
    default:           //
      break;
    }
  }
  predata[3]=predata[2];
  predata[2]=predata[1];
  predata[1]=predata[0];
  predata[0]=recv;
}
