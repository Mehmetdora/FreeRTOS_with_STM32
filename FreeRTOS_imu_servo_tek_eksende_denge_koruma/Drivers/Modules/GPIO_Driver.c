/*
 * GPIO_Driver.c
 *
 *  Created on: Jun 28, 2026
 *      Author: mehmet_dora
 */


#include "GPIO_Driver.h"
#include "stm32f4xx.h"
#include "stdint.h"


// Extern ile bir başka dosyadaki değişken kullanılırken değeri verilmeden sadece tanımlanarak çağırılır
extern SemaphoreHandle_t BtnSmphr_sysOn;
extern SemaphoreHandle_t BtnSmphr_sysOff;
extern SemaphoreHandle_t BtnSmphr_sysRef_start;
extern SemaphoreHandle_t BtnSmphr_sysRef_stop;

uint8_t is_ref_start = 0;




void gpio_setup(void){



	// PC0 -> system on btn  ve PC1 -> system off buton input

	RCC->AHB1ENR |= (1UL << 2);
	RCC->APB2ENR |= (1UL << 14);

	GPIOC->MODER &= ~(3<<(2*0));
	GPIOC->MODER &= ~(3<<(2*1));
	GPIOC->MODER &= ~(3UL << (2 * 13));


	GPIOC->PUPDR &= ~(3<<(2*0));
	GPIOC->PUPDR |=  (1<<(2*0));
	GPIOC->PUPDR &= ~(3<<(2*1));
	GPIOC->PUPDR |=  (1<<(2*1));
	GPIOC->PUPDR &= ~(3UL << (2 * 13));


	GPIOC->OSPEEDR |= (1UL << (2*0));
	GPIOC->OSPEEDR |= (1UL << (2*1));

	SYSCFG->EXTICR[0] &= ~(
	    SYSCFG_EXTICR1_EXTI0 |
	    SYSCFG_EXTICR1_EXTI1);

	SYSCFG->EXTICR[0] |=
	    SYSCFG_EXTICR1_EXTI0_PC |
	    SYSCFG_EXTICR1_EXTI1_PC;

	SYSCFG->EXTICR[3] &= ~(0xFUL << 4);     // EXTI13 temizle
	SYSCFG->EXTICR[3] |=  (2UL << 4);       // EXTI13 -> PC13


	EXTI->FTSR |= (1UL << 0) | (1UL << 1) | (1UL << 13);
	EXTI->IMR |= (1<<0) | (1<<1) | (1UL << 13);



	NVIC_SetPriority(EXTI0_IRQn,6);
	NVIC_EnableIRQ(EXTI0_IRQn);

	NVIC_SetPriority(EXTI1_IRQn,6);
	NVIC_EnableIRQ(EXTI1_IRQn);

	NVIC_SetPriority(EXTI15_10_IRQn, 6);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

}




// PA pinlerine bağlı led varsa bunları kontrol etme
void gpio_set(uint8_t pin_num){
	GPIOA->ODR |= (1UL << pin_num);
}

void gpio_reset(uint8_t pin_num){
	GPIOA->ODR &= ~(1UL << pin_num);
}

void gpio_toggle(uint8_t pin_num){
	GPIOA->ODR ^= (1UL << pin_num);
}



void EXTI0_IRQHandler(){

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI->PR & EXTI_PR_PR0){
		EXTI->PR |= EXTI_PR_PR0;

		xSemaphoreGiveFromISR(BtnSmphr_sysOn, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}

}

void EXTI1_IRQHandler(){

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI->PR & EXTI_PR_PR1){
		EXTI->PR |= EXTI_PR_PR1;

		xSemaphoreGiveFromISR(BtnSmphr_sysOff, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}

}

void EXTI15_10_IRQHandler(){

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(EXTI->PR & EXTI_PR_PR13){
		EXTI->PR |= EXTI_PR_PR13;


		if(is_ref_start == 0){
			xSemaphoreGiveFromISR(BtnSmphr_sysRef_start, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			is_ref_start = 1;
		}else if(is_ref_start == 1){
			xSemaphoreGiveFromISR(BtnSmphr_sysRef_stop, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			is_ref_start = 0;
		}

	}

}



