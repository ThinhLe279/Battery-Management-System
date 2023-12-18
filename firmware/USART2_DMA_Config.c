/*
 * USART2_DMA_config.c
 *
 *  Created on: Dec , 2023
 *      Author: Thinh Le
 */
#include "USART2_DMA_Config.h"
#include "modbus.h"

extern int flag; // global flag for MODBUS activities (first initialized in main.c)

void USART2_Init(void) {
	//enable USART2
	RCC->APB1ENR |= 0x00020000;	 // set bit 17 (USART2 EN)
	RCC->AHBENR |= 0x00000001;	 // enable GPIOA port clock bit 0 (GPIOA EN)
	GPIOA->AFR[0] = 0x00000700;	 // GPIOx_AFRL p.188,AF7 p.177
	GPIOA->AFR[0] |= 0x00007000; // GPIOx_AFRL p.188,AF7 p.177
	GPIOA->MODER |= 0x00000020;	// MODER2=PA2(TX) to mode 10=alternate function mode. p184
	GPIOA->MODER |= 0x00000080;	// MODER2=PA3(RX) to mode 10=alternate function mode. p184

	USART2->BRR = 0x00000116;  // 115200 BAUD and crystal 32MHz. p710, 116
	USART2->CR1 = 0x00000008;  // TE bit. p739-740. Enable transmit
	USART2->CR1 |= 0x00000004; // RE bit. p739-740. Enable receiver
	USART2->CR3 |= (1 << 6); 	// enable DMAR ( DMA reception)
	USART2->CR1 |= 0x00002000; // UE bit. p739-740. Uart enable
}

void DMA_Init(void) {
	// enable DMA1 clock
	RCC->AHBENR |= (1 << 24);
	DMA1_Channel6->CCR &= ~(1 << 0);	// disable channel first
	DMA1_Channel6->CCR |= (1 << 1);	// Transfer complete interrupt enable
	DMA1_Channel6->CCR &= ~(1 << 2);	// Half Transfer interrupt disabled;
	DMA1_Channel6->CCR &= ~(1 << 4); 	// read from Peripheral --> Memory
	DMA1_Channel6->CCR &= ~(1 << 5);	// non-circular mode
	DMA1_Channel6->CCR &= ~(1 << 6); 	// Peripheral increment mode disabled
	DMA1_Channel6->CCR |= (1 << 7);		// Memory increment mode enabled
	DMA1_Channel6->CCR &= ~(3 << 8);	// 8-bit Peripheral data size
	DMA1_Channel6->CCR &= ~(3 << 10);	// 8-bit Memory Data size
	DMA1_Channel6->CCR &= ~(3 << 12);	// Channel Priority level low (PL = 00)
	DMA1_Channel6->CCR &= ~(1 << 14);	// Memory to Memory mode disabled

	DMA1_Channel6->CNDTR = (uint16_t) RX_BUFFER_SIZE;// Number of data items to transfer
	DMA1_Channel6->CPAR = (uint32_t) &USART2->DR;// Source address (USART2 data register)
	DMA1_Channel6->CMAR = (uint32_t) DMA_RX_BUFFER;	// Destination address (DMA_RX_BUFFER)

	NVIC_EnableIRQ(DMA1_Channel6_IRQn);       // Enable DMA1 Channel 6 interrupt
	DMA1_Channel6->CCR |= (1 << 0);		// enable channel
}

void USART2_write(char data) {
	// wait while TX buffer is empty
	while (!(USART2->SR & 0x0080)) {
	}					 // TXE: Transmit data register empty. p736-737
	USART2->DR = (data); // p739
}

char USART2_read() {
	char data = 0;
	// wait while RX buffer data is ready to be read
	while (!(USART2->SR & 0x0020)) {
	}				   // Bit 5 RXNE: Read data register not empty
	data = USART2->DR; // p739
	return data;
}

void DMA1_Channel6_IRQHandler(void) {
	if ((DMA1->ISR) & (1 << 21)) {	// If the Transfer Complete Interrupt is set

		DMA1->IFCR |= (1 << 21);	// Channel 6 transfer complete clear
		if (DMA_RX_BUFFER[0] == SLAVE_ADDR) {
			flag = 1;
		} else
			flag = 2;
		if (DMA_RX_BUFFER[0] == 0x24 && DMA_RX_BUFFER[1] == 0x24) {
			display("detected");
			Restart_DMA();
		}
		USART2->CR1 &= ~(1 << 2); 	// disable RE bit (USART Receiver)
	}
}

void Restart_DMA(void) {
	//re-enable DMA for the next reception
	DMA1_Channel6->CCR &= ~(1 << 0);	// disable channel
	DMA1_Channel6->CNDTR = (uint16_t) RX_BUFFER_SIZE;// Number of data items to transfer
	DMA1_Channel6->CCR |= (1 << 0);		// enable channel
}

void Reset_DMA_RX_BUFFER(void) {
	memset(DMA_RX_BUFFER, '0', RX_BUFFER_SIZE);	// reset DMA buffer for the next reception
}

void display(char* buf) {
	int len = 0;
	while (buf[len] != '\0')
		len++;

	for (int i = 0; i < len; i++) {
		USART2_write(buf[i]);
	}

	USART2_write('\n');
	USART2_write('\r');

}
