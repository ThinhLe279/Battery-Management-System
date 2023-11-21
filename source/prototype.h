
#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_

#include "stm32l1xx.h"

#define TS_CAL1 *((uint16_t *)0x1FF800FA) // TS_CAL1 for the internal temp sensor
#define TS_CAL2 *((uint16_t *)0x1FF800FE) // TS_CAL2 for the internal temp sensor

#define Select_Pin_B 5
#define Select_Pin_A 6



void Adc_set_up();

void Mux_set_up();

void MFET_set_up();

void Select_mux_pin(int channel);

float Read_Cell_Voltage(void);

int Internal_Temp_Read();

int LM35_Temp_read();

void USART2_Init(void);

void USART2_write(char data);

char USART2_read(void);

void delay_Ms(int delay);

void display(char* buf);

#endif /* PROTOTYPE_H_ */
