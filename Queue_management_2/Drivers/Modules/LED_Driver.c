/*
 * LED_Driver.c
 *
 *  Created on: Apr 17, 2026
 *      Author: mehmet_dora
 */

#include "stm32f4xx.h"
#include "LED_DRIVER.h"




void LED_config(void){


	GPIOA->MODER &= ~(3UL << (2*5));
	GPIOA->MODER |= (1UL << (2*5));

}

void LED_ON(void){
	GPIOA->ODR |= (1UL << 5);
}
void LED_OFF(void){
	GPIOA->ODR &= ~(1UL << 5);
}
void LED_TOGGLE(void){
	GPIOA->ODR ^= (1UL << 5);
}

void LED_BLINK(uint16_t blink_ms){

}



