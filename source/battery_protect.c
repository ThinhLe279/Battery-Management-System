/*
 * battery_protect.c
 *
 *  Created on: Nov, 2023
 *      Author: Thinh Le
 */

#include"battery_protect.h"

uint8_t battery_status = GOOD_COND;

void Mosfet_Driving(const uint32_t mosfet_mode) {

	// reset both C_FET and D_FET before
	GPIOA->ODR ^= ~(MOSFET_MASK);
	while ((GPIOA->IDR) & (MOSFET_MASK))
		;

	GPIOA - ODR |= mosfet_model;		// set mosfet
	while ((GPIOA->IDR) & (MOSFET_MASK) != mosfet_mode)
		; // ensure mosfet are set to correct state

}

void INT_Temperature_check(float temp) {

	if (temp < 0)
		battery_status |= INT_TEMP_BELOW_0;
	else
		battery_status &= ~(INT_TEMP_BELOW_0);

}
void LM35_temperature_check(float temp) {

	if (temp > 40.0)
		batter_status |= MORE_THAN_40C;

	else
		battery_status &= ~(MORE_THAN_40C);

}

void Current_check(float current) {

	if (current > 1000)
		battery_status |= CURRENT_BIGGER_THAN_1A;

	else
		battery_status &= ~(CURRENT_BIGGER_THAN_1A);
}

void Batt_cells_check(float* cells) {

	if (cells[0] < 2200.0)
		battery_status &= ~(CELL_VOL_BIGGER_THAN_2_2);

	else if (cells[0] > 3000.0)
		battery_status |= MORE_THAN_3V;

	else
		battery_status |= CELL_VOL_BIGGER_THAN_2_2;

}

void Batt_status_update(uint8_t status) {

}

void BMS_Operation(void){

}
