#include <STC8.H>
#include <string.h>
#include "debug.h"
#include "adc.h"
#include "myprintf.h"
#define RXBUFFERSIZE 1

int *BGV; //�ڲ�Bandgap��ѹֵ�����idata��
//idata��EFH��ַ��Ÿ��ֽ�
//idata��F0H��ַ��ŵ��ֽ�
//��ѹ��λΪ����(mV)

extern code uint16 display0[];
extern code uint16 display1[];
extern code uint16 display2[];
extern code uint16 display3[];


float Variable[20];     //����
float Parameter[20];    //����

uint8 SendPara,send_data_contorl=0,Save_Flag=0;

extern int MYIAP;

char RxBuffer[RXBUFFERSIZE];
uint8 aRxBuffer;
uint8 Uart1_Rx_Cnt = 0;


void my_putchar(uint8 temp)
{
		UartSend(temp);
}
/*����֪ͨ��λ���µ�һ�����ݿ�ʼ��Ҫ�������ݱ��뷢����*/
void Send_Begin()
{ 
  my_putchar(0x55);
  my_putchar(0xaa);
  my_putchar(0xae);
}

//������������ 1
void Testdata_generate()    //generate����
{
	BGV = (int idata *)0xef;
	Variable[0]=0;
	Variable[1]=0;
}

//���ͱ���
// Variable ���� 1
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

//�޸� ����
void Modify_Parameter(uint8 *buff)     //Modify �޸� Parameter����
{
  uint8 i=0,addr=0;
  float temp,example=123.1;
  uint8 Parameter_num=14; //14���ɸĲ���
  /*          �޸Ĳ�������         */
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
/*         �Ӳ��������и��²���ֵ        */
void Save_Parameter()
{
	P43=Parameter[0];
}

//���Ͳ���
void Send_Parameter()
{
  uint8 i=0,ch=0;
  float temp=0;
  uint8 Parameter_num=14;  //14���ɸĲ���
  
  /*         ������ֵ���µ�����������  ʾ��         */
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
  my_putchar(0X0b);//֡β
}

void send_init()//��λ�������ݳ�ʼ��
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
  static uint8 Data_Receiving=0;   //Data_Receiving���ݽ���
  
  //if(uart_query(uart1)==1)  uart_getchar (uart1,(uint8*)(&recv));  //����ʵ�ʵĴ������޸�     uart_query��ѯ�Ƿ���յ�һ���ֽ�     uart_getchar�ȴ����� 1 ���ֽ�
  /**********�������ڽ���������λ���Ĳ�������*********/
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
      data_cnt=0;    //�ﵽ֡��
      Data_Receiving=0;
      if(recv==0xab)  //֡β
      {
        Modify_Parameter(Recv_Buff);
        SendPara=1;      //�����ش���ȷ�ϲ����޸����
				Save_Flag=1;
      }
    }
  }
  else
  {
    // beep=1; //����������
    
    switch(recv)         //�жϹ�����
    {
    case 0x30:           //��ģ��ͣ���� �յ����ݺ���Կ���С����������ͣ
			
      break;
      
    case 0x31:           //���ݷ��Ϳ���
			
      IAP_CONTR = 0x60;//��Ƭ��׼����ʼisp����
		
      /*********�������ݷ���ʱҪռ�ò���CPU��Դ������ʱ�䣬С������ʱ�����鷢�����ݣ����Կ��Թرշ���*********/
      break;
      
    case 0x32:           //��ȡ����
      if(SendPara==0)
        SendPara=1;
      break;        
    case 0X33:             //�޸Ĳ���
      Data_Receiving=1;
      data_cnt=0;
      break;
    case 0X34:             //�������
      break;
      
    case 0X35:             //SD�����ݱ��濪��
      
      break;
      
    case 0X36:             //��ȡSD�����ݱ�����ʼ��������Ϣ
      
      break;
      
    case 0X37:             //SD����һ֡
      
      break;
      
    case 0X38:             //SD����һ֡
      
      break;
      
    case 0X39:             //������ת
      
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
