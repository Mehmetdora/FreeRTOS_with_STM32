/*
 * GPIO_Driver.c
 *
 *  Created on: Jun 28, 2026
 *      Author: mehmet_dora
 */


#include "GPIO_Driver.h"
#include "stm32f4xx.h"


void gpio_setup(void){


	// PA13 - PA14 - PA15 ledler için

	RCC->AHB1ENR |= (1UL << 2);

	GPIOC->MODER &= ~(3UL << (2*0));
	GPIOC->MODER |=  (1UL << (2*0));

	GPIOC->MODER &= ~(3UL << (2*1));
	GPIOC->MODER |=  (1UL << (2*1));

	GPIOC->MODER &= ~(3UL << (2*2));
	GPIOC->MODER |=  (1UL << (2*2));



	GPIOC->OSPEEDR |= (1UL << (2*0));
	GPIOC->OSPEEDR |= (1UL << (2*1));
	GPIOC->OSPEEDR |= (1UL << (2*2));


}

void gpio_set(uint8_t pin_num){
	GPIOC->ODR |= (1UL << pin_num);
}

void gpio_reset(uint8_t pin_num){
	GPIOC->ODR &= ~(1UL << pin_num);
}

