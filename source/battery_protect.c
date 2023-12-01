/*
 * battery_protect.c
 *
 *  Created on: Nov, 2023
 *      Author: Thinh Le
 */

#include"battery_protect.h"
#include"prototype.h"

/*this global variable will be used as a set of flags  to demonstrating
 * the status of the battery according to Int_temp, LM32_Temp, Cell_Voltage and shunt current*/
uint8_t battery_status = 0x00U;

void Mosfet_Driving(uint32_t mosfet_mode) {

	// reset both C_FET and D_FET before
	GPIOA->ODR &= ~(MOSFET_MASK);
	while ((GPIOA->IDR) & (MOSFET_MASK))
		;

	GPIOA->ODR |= mosfet_mode;		// set mosfet
	while (((GPIOA->IDR) & (MOSFET_MASK)) != mosfet_mode)
		; // ensure mosfet are set to correct state !!

}

void INT_Temperature_check(float temp) {

	if (temp < 0)// set bit 2 of the battery_status to 1 if the INT_temp is < 0
		battery_status |= INT_TEMP_BELOW_0;
	else
		// bit set bit 2 of the battery_status to 0 if the INT_temp is < 0
		battery_status &= ~(INT_TEMP_BELOW_0);

}
void LM35_temperature_check(float temp) {

	if (temp > 40.0)//set bit 0 of the battery_status to 1 if the LM35_temp is > 40C
		battery_status |= MORE_THAN_40C;

	else
		////set bit 0 of the battery_status to 0 if the LM35_temp is < 40C
		battery_status &= ~(MORE_THAN_40C);

}

void Current_check(float current) {

	if (current > 1000)	//set bit 4 of the battery_status to 1 if the current > 1A
		battery_status |= CURRENT_BIGGER_THAN_1A;

	else
		//set bit 4 of the battery_status to 0 if the current < 1A
		battery_status &= ~(CURRENT_BIGGER_THAN_1A);
}

void Batt_cells_check(float* cells) {

	if (cells[0] < 2200.0)//set bit 3 of the battery_status to 0 if the cell_vol < 2.2V
		battery_status &= ~(CELL_VOL_BIGGER_THAN_2_2);

	else if (cells[0] > 3000.0)	//set bit 1 of the battery_status to 1 if the cell_vol > 3V
		battery_status |= MORE_THAN_3V;

	else
		//set bit 3 of the battery_status to 1 if the cell_vol > 2.2V
		battery_status |= CELL_VOL_BIGGER_THAN_2_2;

}

void BMS_Operation(void) {

	switch (battery_status) {
	case (0x00U):
		Mosfet_Driving(CHARGE_MODE);
		display("mosfet state: 1 - 0");
		break;

	case (CURRENT_BIGGER_THAN_1A):
		Mosfet_Driving(CLOSED_MODE);
		display("mosfet state: 1 - 1\n\r");
		break;

	case (CELL_VOL_BIGGER_THAN_2_2 | INT_TEMP_BELOW_0):
		Mosfet_Driving(DISCHARGE_MODE);
		display("mosfet state: 0 - 1\n\r");
		break;

	case (CELL_VOL_BIGGER_THAN_2_2):
		Mosfet_Driving(DISCHARGE_MODE);
		display("mosfet state: 0 - 1\n\r");
		break;

	case (CURRENT_BIGGER_THAN_1A | CELL_VOL_BIGGER_THAN_2_2 | INT_TEMP_BELOW_0):
		Mosfet_Driving(CLOSED_MODE);
		display("mosfet state: 1 - 1\n\r");
		break;

	case (CURRENT_BIGGER_THAN_1A | CELL_VOL_BIGGER_THAN_2_2):
		Mosfet_Driving(CLOSED_MODE);
		display("mosfet state: 1 - 1\n\r");
		break;

	default:
		display("mosfet state: 0 - 0");
		Mosfet_Driving(OFF_MODE);
		break;

	}

}
