/*
 * modbus.c
 *
 *  Created on: Dec 6, 2023
 *      Author: levop
 */

#include "ADC_measurement.h"
#include "modbus.h"
#include "USART2_DMA_config.h"

void Modbus_routine(MODBUS* mod_value) {
	// check CRC of the request frame
	unsigned short int Check_CRC = CRC16(DMA_RX_BUFFER, RX_BUFFER_SIZE);
	if (Check_CRC != 0) {
		//display("problem with CRC");
		return;
	}
	//display("CRC is Okay");

	if (DMA_RX_BUFFER[FUNC_CODE_POS] != FUNC_CODE) {
		//display("function code is wrong");
		return;
	}
	//display("function code is okay");

	switch (DMA_RX_BUFFER[STARTING_ADDR_POS]) {
	case READ_INT_TEMP:
		//display("read int temp:");
		Response_frame(mod_value->int_temp);
		break;

	case READ_LM35_TEMP:
		//display("read LM35 Temp");
		Response_frame(mod_value->LM35_temp);
		break;

	case READ_CELL_1:
		//display("read Cell 1:");
		Response_frame(mod_value->cell_vol[0]);
		break;

	case READ_CELL_2:
		//display("read Cell 2:");
		Response_frame(mod_value->cell_vol[1]);
		break;

	case READ_CELL_3:
		//display("read Cell 3:");
		Response_frame(mod_value->cell_vol[2]);
		break;

	case READ_CELL_4:
		//display("read Cell 3:");
		Response_frame(mod_value->cell_vol[3]);
		break;

	case READ_SHUNT:
		//display("read shunt:");
		Response_frame(mod_value->shunt);
		break;
	}

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

void Response_frame(int transfer_data) {

	char response_frame[7] = { SLAVE_ADDR, FUNC_CODE, BYTE_COUNT, 0, 0, 0, 0 };	// structure of response frame
	char sensor_high_byte = 0;
	char sensor_low_byte = 0;
	unsigned short int calculated_CRC = 0;
	char crc_high_byte = 0;
	char crc_low_byte = 0;

	//represent the transfer_data in 2 bytes
	sensor_high_byte = (transfer_data >> 8) | sensor_high_byte;
	sensor_low_byte = transfer_data | sensor_low_byte;
	response_frame[3] = sensor_high_byte;
	response_frame[4] = sensor_low_byte;

	// calculate and represent the CRC in 2 bytes
	calculated_CRC = CRC16(response_frame, 5);
	crc_high_byte = (calculated_CRC >> 8) | crc_high_byte;
	crc_low_byte = calculated_CRC | crc_low_byte;
	response_frame[6] = crc_high_byte;
	response_frame[5] = crc_low_byte;

	// sending response frame:
	display("response frame: ");
	for (int i = 0; i < 7; i++) {
		USART2_write(response_frame[i]);
	}
}
