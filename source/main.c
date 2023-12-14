/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32l1xx.h"
#include "nucleo152start.h"

#include "ADC_measurement.h"
#include "battery_protect.h"
#include "USART2_DMA_Config.h"
#include "modbus.h"

int flag = 0;		// global flag for MODBUS activities
int main(void) {

	__disable_irq();		//global disable IRQs, M3_Generic_User_Guide p135.
	/* Configure the system clock to 32 MHz and update SystemCoreClock */
	SetSysClock();
	SystemCoreClockUpdate();
	USART2_Init();
	DMA_Init();
	__enable_irq();		//global enable IRQs, M3_Generic_User_Guide p135

	Adc_set_up();	// set up ADC for measurements
	Mux_set_up();	// set up for the multiplexer
	MFET_set_up();	// set up C_FET and D_FET

	float Int_temp = 0;
	float LM35_temp = 0;
	float cell_vol[4] = { 0, 0, 0, 0 };
	float shunt = 0;

	MODBUS Mod;		// modbus object

	// reset pin A and B of the multipler
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
			USART2->CR1 |= (1 << 2);	////re-enable USART2 RE
			flag = 0;
			break;

		case 2:
			//display("wrong address");
			Restart_DMA();
			Reset_DMA_RX_BUFFER();
			USART2->CR1 |= (1 << 2);	//re-enable USART2 RE
			flag = 0;
			break;
		}
		shunt = Read_shunt_resistor();
		delay_Ms(200);
		Mod.shunt = (int) (shunt * 10);	// update shunt value for modbus protocol
		Current_check(shunt);// check for batter protection (baterry_protect.c)

		Int_temp = Internal_Temp_Read();
		delay_Ms(200);
		Mod.int_temp = (int) (Int_temp * 10);// update int_temp value for modbus protocol
		INT_Temperature_check(Int_temp);// check for batter protection (baterry_protect.c)

		LM35_temp = LM35_Temp_read();
		delay_Ms(200);
		Mod.LM35_temp = (int) (LM35_temp * 10);	// update LM35_temp value for modbus protocol
		LM35_temperature_check(LM35_temp);// check for batter protection (baterry_protect.c)

		for (int i = 0; i < 4; i++) {
			Select_mux_pin(i);		// select pin in multiplexer
			cell_vol[i] = Read_Cell_Voltage();
			delay_Ms(200);
			Mod.cell_vol[i] = (int) (cell_vol[i] * 10);	// update cells voltage value for modbus protocol
		}
		Batt_cells_check(cell_vol);	// check for batter protection (baterry_protect.c)

		BMS_Operation();// batter protection activities after sensing all the value, (battery_protect.c)

		delay_Ms(1000);

	}
	return 0;
}
