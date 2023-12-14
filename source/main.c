/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32l1xx.h"
#include "nucleo152start.h"

#include "ADC_measurement.h"
#include "battery_protect.h"
#include "USART2_DMA_Config.h"
#include "modbus.h"



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
	GPIOA->ODR &= ~(1 << Select_Pin_B);
	GPIOA->ODR &= ~(1 << Select_Pin_A);
	/* Infinite loop */
	while (1) {


		switch (flag) {
		case 0:
			break;

		case 1:
			//display("slave address is correct");
			Modbus_routine(&Mod);
			Restart_DMA();
			Reset_DMA_RX_BUFFER();
			USART2->CR1 |= (1 << 2);	////re-enable RE
			flag = 0;
			break;

		case 2:
			//display("wrong address");
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


		BMS_Operation();

		delay_Ms(1000);

	}
	return 0;
}
