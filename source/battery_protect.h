/*
 * battery_protect.h
 *
 *  Created on: Nov , 2023
 *      Author: Thinh Le
 */

#ifndef SOURCE_BATTERY_PROTECT_H_
#define SOURCE_BATTERY_PROTECT_H_

#include<stdint.h>

#define C_FET							(1 << 8)	// PA8	D7
#define D_FET							(1 << 9)	// PA9	D8

// mode of BMS bit 8 and 9 indicate current status of C_FET and D_FET
#define CHARGE_MODE						(C_FET)
#define DISCHARGE_MODE					(D_FET)
#define OFF_MODE						~(C_FET | D_FET)
#define CLOSED_MODE						(C_FET | D_FET)

// Flags indicate battery status

#define MORE_THAN_40C					0x01U	// bit 0 define if the EXT is >40C or not (1 if >40C)
#define MORE_THAN_3V					0x02U	// bit 1 defines if any voltage of battery cells if >3V or not (1 if >3)

#define INT_TEMP_BELOW_0				0x04U	// bit 2 defines if the int is in -20C to -1C or 0C to 40C range (1 > below 0)

#define CELL_VOL_BIGGER_THAN_2_2		0x08U	// bit 3 defines if the cell_vol > 2.2 or not	(1 if >2.2)

#define CURRENT_BIGGER_THAN_1A			0x10U	// bit 4 defines if the shunt current >1A or not (1 if > 1A)



// function prototype

void Mosfet_Driving(const uint32_t mosfet_mode);
void INT_Temperature_check(float temp);
void LM35_temperature_check(float temp);
void Current_check(float current);
void Batt_cells_check(float* cells);
void BMS_Operation(void);

void Batt_status_update(uint8_t status);

#endif /* SOURCE_BATTERY_PROTECT_H_ */
