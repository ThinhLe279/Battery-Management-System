/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32l1xx.h"
#include "nucleo152start.h"

#include "ADC_measurement.h"
#include "battery_protect.h"
#include "USART2_DMA_Config.h"
#include "modbus.h"

//#define MODBUS_TEST

int flag = 0;
int main(void) {

	__disable_irq();		//global disable IRQs, M3_Generic_User_Guide p135.
	/* Configure the system clock to 32 MHz and update SystemCoreClock */
	SetSysClock();
	SystemCoreClockUpdate();
	USART2_Init();
	DMA_Init();
	__enable_irq();		//global enable IRQs, M3_Generic_User_Guide p135

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

#ifdef	MODBUS_TEST
		switch (flag) {
			case 0:
			break;

			case 1:
			display("slave address is correct");
			Modbus_routine(&Mod);
			Restart_DMA();
			Reset_DMA_RX_BUFFER();
			USART2->CR1 |= (1 << 2);	////re-enable RE

			flag = 0;
			break;

			case 2:
			display("wrong address");
			Restart_DMA();
			Reset_DMA_RX_BUFFER();
			USART2->CR1 |= (1 << 2);//re-enable RE
			flag = 0;
			break;
		}

		shunt = 853.4;
		Mod.shunt = (int)(shunt * 10);

		Int_temp = 24.4;
		Mod.int_temp = (int) (Int_temp * 10);

		LM35_temp = 25.6;
		Mod.LM35_temp = (int) (LM35_temp * 10);

		cell_vol[0] = 2000.0;
		Mod.cell_vol[0] = (int) (cell_vol[0] * 10);

		cell_vol[1] = 2100.0;
		Mod.cell_vol[1] = (int) (cell_vol[1] * 10);

		cell_vol[2] = 2200.0;
		Mod.cell_vol[2] = (int) (cell_vol[2] * 10);

		cell_vol[3] = 2300.0;
		Mod.cell_vol[3] = (int) (cell_vol[3] * 10);

		//display("sensing.....");
		delay_Ms(1000);

#else
		switch (flag) {
		case 0:
			break;

		case 1:
			display("slave address is correct");
			Modbus_routine(&Mod);
			Restart_DMA();
			Reset_DMA_RX_BUFFER();
			USART2->CR1 |= (1 << 2);	////re-enable RE
			flag = 0;
			break;

		case 2:
			display("wrong address");
			Restart_DMA();
			Reset_DMA_RX_BUFFER();
			USART2->CR1 |= (1 << 2);	//re-enable RE
			flag = 0;
			break;
		}
		shunt = Read_shunt_resistor();
		delay_Ms(200);
		Mod.shunt = (int) (shunt * 10);
		Current_check(shunt);

		Int_temp = Internal_Temp_Read(); // remember to divide this temp by 10.
		delay_Ms(200);
		Mod.int_temp = (int) (Int_temp * 10);
		INT_Temperature_check(Int_temp);

		LM35_temp = LM35_Temp_read();
		delay_Ms(200);
		Mod.LM35_temp = (int) (LM35_temp * 10);
		LM35_temperature_check(LM35_temp);

		for (int i = 0; i < 4; i++) {
			Select_mux_pin(i);
			cell_vol[i] = Read_Cell_Voltage();
			delay_Ms(200);
			Mod.cell_vol[i] = (int) (cell_vol[i] * 10);
		}
		Batt_cells_check(cell_vol);

		sprintf(buf,
				"LM35 Temp: %d C \n\rInternal Temp: %d C\n\rCell 1 (mV): %d\n\rcell 2 (mV): %d\n\rcell 3 (mV): %d\n\rcell 4 (mV): %d\n\rCurrent (mA): %d\n\r",
				(int) LM35_temp, (int) Int_temp, (int) cell_vol[0],
				(int) cell_vol[1], (int) cell_vol[2], (int) cell_vol[3],
				(int) shunt);

		display(buf);
		BMS_Operation();

		delay_Ms(1000);
#endif

	}
	return 0;
}
