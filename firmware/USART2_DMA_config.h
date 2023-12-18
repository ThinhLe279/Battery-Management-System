/*
 * USART2_DMA_config.h
 *
 *  Created on: Dec , 2023
 *      Author: Thinh Le
 */

#ifndef SOURCE_USART2_DMA_CONFIG_H_
#define SOURCE_USART2_DMA_CONFIG_H_

#include "stm32l1xx.h"
#include<stdint.h>
#include<string.h>

#define RX_BUFFER_SIZE	8

char DMA_RX_BUFFER[RX_BUFFER_SIZE];		// buffer to store bytes from Master

void USART2_Init(void);

void DMA_Init(void);

void USART2_write(char data);

char USART2_read(void);

void DMA1_Channel6_IRQHandler(void);

void Restart_DMA(void);

void Reset_DMA_RX_BUFFER(void);

void display(char* buf);

#endif /* SOURCE_USART2_DMA_CONFIG_H_ */
