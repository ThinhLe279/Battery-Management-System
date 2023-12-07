/*
 * modbus.h
 *
 *  Created on: Nov, 2023
 *      Author: Thinh Le
 */

#ifndef SOURCE_MODBUS_H_
#define SOURCE_MODBUS_H_
#include<stdio.h>
/*request frame structure:
 *  [SLAVE_addr (0x04) ; Function code (0x04) ; 0x00 ; X ; 0x00 ; 0x01 ; CRC ; CRC]
 *
 *  value of X:
 *  0x01: READ_INT_TEMP
 *  0x02: READ_LM35_TEMP
 *  0x03: READ_CELL_1
 *  0x04: READ_CELL_2
 *  0x05: READ_CELL_3
 *  0x06: READ_CELL_4
 *  0x07: READ_SHUNT*/
extern int flag;

/*response frame structure:
 * [slave_addr; function code; byte count (0x02); Reg_val_1; reg_val_2; CRC(higher); CRC(lower) ]*/

struct MODBUS_DATA_STOGAGE {
	int int_temp ;
	int LM35_temp;
	int shunt ;
	int cell_vol[4];
}typedef MODBUS;

// slave address, function code and byte count in response frame
#define SLAVE_ADDR		0x04
#define FUNC_CODE		0x04
#define BYTE_COUNT		0x02
// starting address option
#define READ_INT_TEMP 	0x01
#define	READ_LM35_TEMP	0x02
#define READ_CELL_1		0x03
#define READ_CELL_2		0x04
#define READ_CELL_3		0x05
#define READ_CELL_4		0x06
#define READ_SHUNT		0x07

#define FUNC_CODE_POS		1	// position of function code in request frame
#define STARTING_ADDR_POS	3	// position of the starting address in the request frame






void USART2_IRQHandler(void);
void Modbus_routine(MODBUS* mod_value);
unsigned short int CRC16(char *nData, unsigned short int wLength);
void Response_frame(int senor_median_value);



#endif /* SOURCE_MODBUS_H_ */
