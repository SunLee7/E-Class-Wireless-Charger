#ifndef _DEBUG_H_
#define _DEBUG_H_

#define BYTE3(Temp)       (*(char *)(&Temp))     
#define BYTE2(Temp)       (*((char *)(&Temp) + 1))
#define BYTE1(Temp)       (*((char *)(&Temp) + 2))
#define BYTE0(Temp)       (*((char *)(&Temp) + 3))

typedef unsigned char       uint8;   //  8 bits 
typedef unsigned short int  uint16;  // 16 bits 

typedef char                int8;    //  8 bits 
typedef short int           int16;   // 16 bits 

extern float Variable[20];
extern float Parameter[20];
extern uint8 aRxBuffer;
extern uint8 SendPara,send_data_contorl,Save_Flag;


//void Send_Inductor();
void Testdata_generate();
//void Send_SD();
void Send_Begin();
void Send_Parameter();
void Send_Variable();
void send_init();
void Save_Parameter();
void my_debug(unsigned char tmp);

#endif
