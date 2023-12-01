/*
 * modbus.c
 *
 *  Created on: Nov , 2023
 *      Author: Thinh Le
 */
#include"modbus.h"
#include"prototype.h"

//#define DEBUG_MODBUS

char MFlag = 0;

char Received_frame[8] = { 0 };
int Received_frame_len = 8;

char buf[200] = { 0 };



void USART2_IRQHandler(void) {
	char received_slave_address = 0;
#ifdef DEBUG_MODBUS
	display("interrupt occur");
#endif
	if (USART2->SR & 0x0020) 		//if data available in DR register. p737
			{
		received_slave_address = USART2->DR;
	}

	if (received_slave_address == SLAVE_ADDR) //if we have right address
	{
#ifdef DEBUG_MODBUS
		display("Slave address is correct, set MFlag to 1");
#endif
		MFlag = 1;
		GPIOA->ODR |= 0x20;				//0010 0000 or bit 5. p186

	} else {
#ifdef DEBUG_MODBUS
		display("Slave address is wrong, set MFlag to 2");
#endif
		MFlag = 2;
		GPIOA->ODR &= ~0x20;				//0010 0000 ~and bit 5. p186
	}
	USART2->CR1 &= ~0x0020;	//Disable USARTx interrupt (RXNEIE interrupt disable)

#ifdef DEBUG_MODBUS
	display("Jumping out of Interrupt function");
#endif
}

void Check_modbus_state(MODBUS* Object) {
#ifdef DEBUG_MODBUS
	display("entered Check_modbus_state function.");
#endif
	switch (MFlag) {

	case 0:
#ifdef DEBUG_MODBUS
		display("MFlag is 0, resetting received_frame");
#endif
		Reset_received_frame();
		break;

	case 1:
#ifdef DEBUG_MODBUS
		display("Mflag is 1, now will jump into Modbus_routine");
#endif
		Modbus_routine(Object);
		MFlag = 0;
		USART2->CR1 |= 0x0020;			//enable RX interrupt
		break;

	case 2:
		wrong_slave();
		break;
	}

}
void Modbus_routine(MODBUS* mod_value) {
#ifdef DEBUG_MODBUS
	display("entered modbus_routine");
#endif
	Received_frame[0] = SLAVE_ADDR;
#ifdef DEBUG_MODBUS
	display("Now will take remaining 7 bytes of request frame");
#endif
	for (int i = 1; i < Received_frame_len; i++) {
#ifdef DEBUG_MODBUS
		display("entered loop for taking 7 btyes.....");
#endif
		if (!(USART2->SR & 0x0020)){
			display("RX buffer is empty...");
		}
		Received_frame[i] = USART2_read();// take the remaining 7 bytes of request frame;
		USART2_write(Received_frame[i]);
	}

#ifdef DEBUG_MODBUS
	display("now display the full request frame ");
	for (int i = 0; i < Received_frame_len; i++) {
		USART2_write(Received_frame[i]);
	}
#endif

	unsigned short int CRC2 = CRC16(Received_frame, Received_frame_len);

#ifdef DEBUG_MODBUS
	sprintf(buf, "CRC value: %d", CRC2);
	display(buf);
#endif
	delay_Ms(10);

	if (CRC2 == 0) {
		int data = 0;
		switch (Received_frame[3]) {

		case READ_INT_TEMP:
#ifdef DEBUG_MODBUS
			display("reading INT temp.....");
#endif
			data = mod_value->int_temp;
			Response_frame(data);
			break;

		case READ_LM35_TEMP:
#ifdef DEBUG_MODBUS
			display("reading LM35 temp.....");
#endif
			data = mod_value->LM35_temp;
			Response_frame(data);
			break;

		case READ_CELL_1:
#ifdef DEBUG_MODBUS
			display("reading Cell_voll 1.....");
#endif
			data = mod_value->cell_vol[0];
			Response_frame(data);
			break;

		case READ_CELL_2:
#ifdef DEBUG_MODBUS
			display("reading Cell_voll 2.....");
#endif
			data = mod_value->cell_vol[1];
			Response_frame(data);
			break;

		case READ_CELL_3:
#ifdef DEBUG_MODBUS
			display("reading Cell_voll 3.....");
#endif
			data = mod_value->cell_vol[2];
			Response_frame(data);
			break;

		case READ_CELL_4:
#ifdef DEBUG_MODBUS
			display("reading Cell_voll 4.....");
#endif
			data = mod_value->cell_vol[4];
			Response_frame(data);
			break;

		case READ_SHUNT:
#ifdef DEBUG_MODBUS
			display("reading Shunt current.....");
#endif
			data = mod_value->shunt;
			Response_frame(data);
			break;
		default:
			display("non-existing input register");
			break;
		}

	}
#ifdef DEBUG_MODBUS
	display("jumping out of modbus_routine");
#endif

}

unsigned short int CRC16(char *nData, unsigned short int wLength) {
	uint8_t i, j;
	unsigned short int crc = 0xFFFF;
	for (i = 0; i < wLength; i++) {
		crc ^= nData[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ 0xA001;
			} else {
				crc = crc >> 1;
			}
		}
	}
	return crc;
}

int check_input_reg(char rec) {
	int check;
	if (rec == READ_INT_TEMP) {
		check = 1;

	} else
		check = 0;
	return check;
}

void wrong_slave(void) {

	USART2->CR1 &= ~0x00000004;		//RE bit. p739-740. Disable receiver
	delay_Ms(10); 					//time=1/9600 x 10 bits x 7 byte = 7,29 ms
	USART2->CR1 |= 0x00000004;		//RE bit. p739-740. Enable receiver
	USART2->CR1 |= 0x0020;			//enable RX interrupt
	MFlag = 0;
}

void Reset_received_frame(void) {
	for (int i = 0; i < Received_frame_len; i++) {
		Received_frame[i] = '0';
	}
}

void Response_frame(int sensor_median_value) {

	GPIOA->ODR |= 0x20;     // LED on while transmitting control RE/TE
	//USART1->CR1 &= ~0x04;   // disable RE pin of USART1
	//USART1->CR1 = 0x08;		// re-enable TE pin of USART1

	char response_frame[7] = { SLAVE_ADDR, 0x04, 0x02, 0, 0, 0, 0 };
	char sensor_high_byte = 0;
	char sensor_low_byte = 0;
	unsigned short int calculated_CRC = 0;
	char crc_high_byte = 0;
	char crc_low_byte = 0;

	sensor_high_byte = (sensor_median_value >> 8) | sensor_high_byte;
	sensor_low_byte = sensor_median_value | sensor_low_byte;

	response_frame[3] = sensor_high_byte;
	response_frame[4] = sensor_low_byte;

	calculated_CRC = CRC16(response_frame, 5);
	crc_high_byte = (calculated_CRC >> 8) | crc_high_byte;
	crc_low_byte = calculated_CRC | crc_low_byte;
#ifdef DEBUG_MODBUS
	sprintf(buf, "CRC for respond frame value: %d", calculated_CRC);
	display(buf);
#endif
	response_frame[6] = crc_high_byte;
	response_frame[5] = crc_low_byte;

	// sending response frame:
	for (int i = 0; i < 7; i++) {
		USART2_write(response_frame[i]);
	}

	GPIOA->ODR &= ~0x20; 		// LED off, turn to receiving mode
	//USART1->CR1 &= ~0x08;   // disable TE pin of USART1
	//USART1->CR1 |= 0x04 ;  	// re-enable RE pin of USART1
}
