/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "prototype.h"
#include "battery_protect.h"
#include "nucleo152start.h"

//#define TEST_BMS

int main(void) {

	/* Configure the system clock to 32 MHz and update SystemCoreClock */
	SetSysClock();
	SystemCoreClockUpdate();
	USART2_Init();
	Adc_set_up();
	Mux_set_up();
	MFET_set_up();

	int Int_temp = 0;
	int LM35_temp = 0;
	float shunt = 0;

	char buf[200];	// contain transfering data

	float cell_vol[4] = { 0, 0, 0, 0 };
	float battery_vol = 0;
	float shunt_resistor = 0.01;
	float current = 0;

	float shunt_arr[5] = {0, 0, 0, 0, 0};

	GPIOA->ODR &= ~(1 << Select_Pin_B);
	GPIOA->ODR &= ~(1 << Select_Pin_A);
	/* Infinite loop */
	while (1) {

#ifndef TEST_BMS
		shunt = Read_shunt_resistor();
		delay_Ms(200);
		Int_temp = Internal_Temp_Read();
		delay_Ms(200);
		LM35_temp = LM35_Temp_read();
		delay_Ms(200);

		for (int i = 0; i < 4; i++) {
			Select_mux_pin(i);
			cell_vol[i] = Read_Cell_Voltage();
			delay_Ms(200);
		}


		sprintf(buf,
				"LM35 Temp: %d C \n\rInternal Temp: %d C\n\rCell 1 (mV): %d\n\rcell 2 (mV): %d\n\rcell 3 (mV): %d\n\rcell 4 (mV): %d\n\rCurrent (mA): %d\n\r",
				LM35_temp, Int_temp, (int) cell_vol[0], (int) cell_vol[1],
				(int) cell_vol[2], (int) cell_vol[3], (int)shunt);


		display(buf);

		delay_Ms(1000);
#else
		Int_temp = 20;
		INT_Temperature_check((float)Int_temp);
		LM35_temp = 25;
		LM35_temperature_check((float)LM35_temp);

		float cel = 2400.5;
		cell_vol[0] = cel;
		cell_vol[1] = cel;
		cell_vol[2] = cel;
		cell_vol[3] = cel;
		Batt_cells_check (cell_vol);
		shunt = 1200.5;
		Current_check(shunt);
		BMS_Operation();
		display("complete testing");
		delay_Ms(1000);
		delay_Ms(1000);
#endif
	}
	return 0;
}
