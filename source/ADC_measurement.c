/*
 * function.c
 *
 *  Created on: Oct 24, 2023
 *      Author: levop
 */
#include "ADC_measurement.h"

/* Define an array for select pins */
const int selectPins[2] = { Select_Pin_A, Select_Pin_B };

void Adc_set_up() {

	// set Analog pin
	RCC->AHBENR |= 1;					// enable GPIOA clock
	GPIOA->MODER |= 0x3;				// PA0 for LM35		A0
	GPIOA->MODER |= (1 << 3) | (1 << 2); // PA1 for the multiplexer		A1
	GPIOA->MODER |= (1 << 9) | (1 << 8); // PA4 for the Shunt resistor.		A2
	// setup ADC1 and temp sensor. p287
	RCC->APB2ENR |= 0x00000200; // enable ADC1 clock
	// set the resolution
	ADC1->CR1 &= ~0x03000000; // resolution 12-bit
	// set the sampling rate
	ADC1->SMPR3 = 7;		// sampling time longest 384 cycles for channel 0
	ADC1->SMPR3 |= (1 << 3) | (1 << 4) | (1 << 5);// sampling time longest 384 cycles for channel 1
	ADC1->SMPR3 |= (1 << 12) | (1 << 13) | (1 << 14);// sampling time longest 384 cycles for channel 4
	ADC1->SMPR2 |= (1 << 18) | (1 << 19) | (1 << 20); // sampling time longest 384 cycles for channel 16
	// set single-mode , bit 11 = 0 align right
	ADC1->CR2 = 0;
	// enable temp sensor
	ADC->CCR |= (1 << 23);
}

void Mux_set_up() {
	// set select pins

	GPIOA->MODER |= (1 << 10); // pin PA5 as Output   D13 	(B)
	GPIOA->MODER |= (1 << 12); // pin PA6 as Output   D12	(A)
}

void MFET_set_up() {
	GPIOA->MODER |= (1 << 16); // pin PA8 for C_FET  D7
	GPIOA->MODER |= (1 << 18); // pin PA9 for D_FET  D8
}

void Select_mux_pin(int channel) {
	for (int i = 0; i < 2; i++) {
		if (channel & (1 << i)) {
			GPIOA->ODR |= (1 << selectPins[i]);
		} else
			GPIOA->ODR &= ~(1 << selectPins[i]);
	}
}

float Read_Cell_Voltage(void) { // this functions return voltage of a cell in mV
	int result = 0;
	float mili_volt = 0;
	ADC1->SQR5 = 1;	// Set the conversion sequence to start at the specified channel
	ADC1->CR2 |= 1;			 // Bit 0, ADC on/off (1=on, 0=off)
	ADC1->CR2 |= 0x40000000; // Start conversion
	while (!(ADC1->SR & 2)) {
	}				   // Wait for conversion complete
	result = ADC1->DR; // Read conversion result
	//char buf[100];
	//sprintf(buf, "ADC of Reading cell function: %d\n\r", result);
	//display(buf);

	mili_volt = ((result * 3.3) / 4095.0) * 1000;

	ADC1->CR2 &= ~1; // bit 0, ADC on/off (1=on, 0=off)

	return mili_volt;
}

float Read_shunt_resistor(void) {	// this function to read the current from Shunt ( in mA)

	int result = 0;
	ADC1->SQR5 = 4;	// Set the conversion sequence to start at the specified channel
	ADC1->CR2 |= 1;			 // Bit 0, ADC on/off (1=on, 0=off)
	ADC1->CR2 |= 0x40000000; // Start conversion
	while (!(ADC1->SR & 2)) {
	}				   // Wait for conversion complete
	result = ADC1->DR; // Read conversion result
	// I just return the ADC value of this channel, you need to apply your fomula to calculate the current

	ADC1->CR2 &= ~1; // bit 0, ADC on/off (1=on, 0=off)
	float vol = (result / 4095.0) * 3.3;
	float value1 = vol / 700.0;
	float current = value1 / 0.01;

	return current * 1000;

}

float Internal_Temp_Read() {
	int result = 0;	 // store adc value
	ADC1->SQR5 = 16; // SQ1 for channel 16 (internal sensor)
	ADC1->CR2 |= 1;	 // bit 0, ADC on/off (1=on, 0=off)

	ADC1->CR2 |= 0x40000000; // start conversion
	while (!(ADC1->SR & 2))
		; // wait for conversion to complete

	result = ADC1->DR;

	float result2 = (110.0 - 30.0) / ((float) TS_CAL2 - (float) TS_CAL1)
			* ((float) result - (float) TS_CAL1) + 50.0;

	ADC1->CR2 &= ~1; // bit 0, ADC on/off (1=on, 0=off)

	return result2;
}

float LM35_Temp_read() {
	int result = 0;
	ADC1->SQR5 = 0;			 // SQ1 for channel (A0)
	ADC1->CR2 |= 1;			 // bit 0, ADC on/off (1=on, 0=off)
	ADC1->CR2 |= 0x40000000; // start conversion
	while (!(ADC1->SR & 2))
		; // wait for conversion to complete

	result = ADC1->DR;

	float voltage = (result / 4095.0) * 3.3;
	//voltage *= 100;					 // to get rid of decimal
	//int final_deg = (voltage) / 100; // to grab integer part
	float temp = voltage * 13.33;
	//int final_deg_dec = ((int) deg_dec % 100); // to grab decimal part
	//final_deg_dec = (final_deg_dec * 13.33) / 100;
	//int final_value = final_deg + final_deg_dec;

	ADC1->CR2 &= ~1; // bit 0, ADC on/off (1=on, 0=off)

	return temp;
}

void delay_Ms(int delay) {
	int i = 0;
	for (; delay > 0; delay--)
		for (i = 0; i < 2460; i++)
			; // measured with oscilloscope
}


