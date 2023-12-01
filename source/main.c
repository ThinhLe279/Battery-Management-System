/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32l1xx.h"
#include "prototype.h"
#include "battery_protect.h"
#include "modbus.h"
#include "nucleo152start.h"

//#define TEST_BMS
//#define MODBUS_TEST
int main(void) {

	__disable_irq();		//global disable IRQs, M3_Generic_User_Guide p135.
	/* Configure the system clock to 32 MHz and update SystemCoreClock */
	SetSysClock();
	SystemCoreClockUpdate();
	USART2_Init();
	// enable USART2 interrupt
	USART2->CR1 |= 0x0020;			//enable RX interrupt
	NVIC_EnableIRQ(USART2_IRQn); 	//enable interrupt in NVIC
	__enable_irq();				//global enable IRQs, M3_Generic_User_Guide p135

	Adc_set_up();
	Mux_set_up();
	MFET_set_up();

	float Int_temp = 0;
	float LM35_temp = 0;
	float cell_vol[4] = { 0, 0, 0, 0 };
	float shunt = 0;

	MODBUS Mod;

	char buf[200];	// contain transfering data


	GPIOA->ODR &= ~(1 << Select_Pin_B);
	GPIOA->ODR &= ~(1 << Select_Pin_A);
	/* Infinite loop */
	while (1) {

#ifndef TEST_BMS
#ifdef	MODBUS_TEST
		Check_modbus_state(&Mod);
		shunt = 853.4;
		Mod.shunt = shunt;

		Int_temp = 24.4;
		Mod.int_temp = (int)(Int_temp*10);

		LM35_temp = 25.6;
		Mod.LM35_temp = (int)(LM35_temp*10);

		cell_vol[0] = 2000.0;
		Mod.cell_vol[0] = (int)(cell_vol[0]*10);

		cell_vol[1] = 2100.0;
		Mod.cell_vol[1] = (int)(cell_vol[1]*10);

		cell_vol[2] = 2200.0;
		Mod.cell_vol[2] = (int)(cell_vol[2]*10);

		cell_vol[3] = 2300.0;
		Mod.cell_vol[3] = (int)(cell_vol[3]*10);

		display("sensing.....");
		delay_Ms(1000);

#else
		shunt = Read_shunt_resistor();
		delay_Ms(200);
		Current_check(shunt);

		Int_temp = Internal_Temp_Read(); // remember to divide this temp by 10.
		delay_Ms(200);
		INT_Temperature_check(Int_temp);

		LM35_temp = LM35_Temp_read();
		delay_Ms(200);
		LM35_temperature_check(LM35_temp);


		for (int i = 0; i < 4; i++) {
			Select_mux_pin(i);
			cell_vol[i] = Read_Cell_Voltage();
			delay_Ms(200);
		}
		Batt_cells_check (cell_vol);

		sprintf(buf,
				"LM35 Temp: %d C \n\rInternal Temp: %d C\n\rCell 1 (mV): %d\n\rcell 2 (mV): %d\n\rcell 3 (mV): %d\n\rcell 4 (mV): %d\n\rCurrent (mA): %d\n\r",
				(int)LM35_temp, (int)Int_temp, (int) cell_vol[0], (int) cell_vol[1],
				(int) cell_vol[2], (int) cell_vol[3], (int) shunt);


		display(buf);
		BMS_Operation();


		delay_Ms(1000);
#endif

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
