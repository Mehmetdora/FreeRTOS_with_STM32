/*
 * GPIO_Driver.c
 *
 *  Created on: Jun 28, 2026
 *      Author: mehmet_dora
 */


#include "GPIO_Driver.h"
#include "stm32f4xx.h"





void gpio_setup_leds(void){



	// PB pinleri output

	RCC->AHB1ENR |= (1UL << 1);

	GPIOB->MODER |= ((1UL<<(2*0)) | (1UL<<(2*1)) | (1UL<<(2*2)) | (1UL<<(2*10)) | (1UL<<(2*12)) | (1UL<<(2*13)) | (1UL<<(2*14)) | (1UL<<(2*15)) );

	GPIOC->OSPEEDR |= ((1UL<<(2*0)) | (1UL<<(2*1)) | (1UL<<(2*2)) | (1UL<<(2*10)) | (1UL<<(2*12)) | (1UL<<(2*13)) | (1UL<<(2*14)) | (1UL<<(2*15)) );


}

void gpio_setup_adc(int resolution){


	// RCC init()
	RCC->APB2ENR |= (1UL << 8);	// ADC1 için clock enable
	RCC->AHB1ENR |= (1UL << 0);	// GPIOA için clock enable


	GPIOA->MODER |= (3UL << 0);	// PA0 analog mode set



	// SET ADC1 , and wait a bit
	ADC1->CR2 |= (1UL << 0);



	// Chanel Select
	ADC1->SQR3 = 0;		//PA0 kullanılacağı için 0. chanel ayarlandı, datasheet de listesi var

	// Resolution
	uint32_t resolution_bit = 0; 	// input olarak belirlenen değere göre resolution seçimi
	switch(resolution){
	case 12:
		resolution_bit = 0;
		break;
	case 10:
		resolution_bit = 1;
		break;
	case 8:
		resolution_bit = 2;
		break;
	case 6:
		resolution_bit = 3;
		break;
	}
	ADC1->CR1 |= (resolution_bit << 24);

	// Sample time (Channel 0)
	ADC1->SMPR2 |= (7UL << 0);


	//ADC1->CR2 |= (1UL << 0);

}


int ADC_get_analog_value(void){

	int ADC_value = 0;

	ADC1->CR2 |= (1UL << 30);	// ADC1 için start komutu

	while(!(ADC1->SR & (1UL << 1)));	//EOC-> Analog conversion bitene kadar bekle

	ADC_value = ADC1->DR;

	return ADC_value;
}


void gpio_set(uint8_t pin_num){
	GPIOC->ODR |= (1UL << pin_num);
}

void gpio_reset(uint8_t pin_num){
	GPIOC->ODR &= ~(1UL << pin_num);
}




