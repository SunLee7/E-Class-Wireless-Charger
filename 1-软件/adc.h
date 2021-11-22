#ifndef _ADC_H_
#define _ADC_H_

#include "debug.h"

extern uint16 VP13;
extern uint16 VP12;
extern uint16 VP15;
extern uint16 VP14;
extern uint16 VP00;
extern uint16 VP01;
extern uint16 VP02;
void ADCInit(void);
uint16 ReadADC(uint8 ch);

#endif