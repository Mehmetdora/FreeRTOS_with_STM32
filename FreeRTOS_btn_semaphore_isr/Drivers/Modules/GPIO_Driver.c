/*
 * GPIO_Driver.c
 *
 *  Created on: Jun 28, 2026
 *      Author: mehmet_dora
 */


#include "GPIO_Driver.h"
#include "stm32f4xx.h"



// Extern ile bir başka dosyadaki değişken kullanılırken değeri verilmeden sadece tanımlanarak çağırılır
extern SemaphoreHandle_t ButtonOpenSemaphore;
extern SemaphoreHandle_t ButtonCloseSemaphore;




void gpio_setup(void){



	// PC0 ve PC1 buton input

	RCC->AHB1ENR |= (1UL << 2);
	RCC->APB2ENR |= (1UL << 14);

	GPIOC->MODER &= ~(3<<(2*0));
	GPIOC->MODER &= ~(3<<(2*1));

	GPIOC->PUPDR &= ~(3<<(2*0));
	GPIOC->PUPDR |=  (1<<(2*0));
	GPIOC->PUPDR &= ~(3<<(2*1));
	GPIOC->PUPDR |=  (1<<(2*1));

	GPIOC->OSPEEDR |= (1UL << (2*0));
	GPIOC->OSPEEDR |= (1UL << (2*1));

	SYSCFG->EXTICR[0] &= ~(
	    SYSCFG_EXTICR1_EXTI0 |
	    SYSCFG_EXTICR1_EXTI1);

	SYSCFG->EXTICR[0] |=
	    SYSCFG_EXTICR1_EXTI0_PC |
	    SYSCFG_EXTICR1_EXTI1_PC;

	EXTI->FTSR |= (1UL << 0) | (1UL << 1);

	EXTI->IMR |= (1<<0) | (1<<1);



	NVIC_SetPriority(EXTI0_IRQn,6);
	NVIC_EnableIRQ(EXTI0_IRQn);

	NVIC_SetPriority(EXTI1_IRQn,6);
	NVIC_EnableIRQ(EXTI1_IRQn);


}

void gpio_set(uint8_t pin_num){
	GPIOC->ODR |= (1UL << pin_num);
}

void gpio_reset(uint8_t pin_num){
	GPIOC->ODR &= ~(1UL << pin_num);
}







// EXTI GPIO interrupt
/*
 * ISR interrupt içinde freertos un fromisr() uzantılı api'leri kullanılıyorsa bu interrupt ın
 * priorty değeri configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY(FreeRTOSConfig.h içindeki) değerinden
 * daha yüksek olamaz
 *
 * stm32 de küçük priorty değeri daha öncelikli demek anlamına gelir,
 * freeRTOS ta tam tersidir
 *
 */


void EXTI0_IRQHandler(void){


	BaseType_t xHigherPriorityTaskWoken = pdFALSE;


	if(EXTI->PR & EXTI_PR_PR0){

		EXTI->PR |= EXTI_PR_PR0;


		xSemaphoreGiveFromISR(ButtonOpenSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}

}

void EXTI1_IRQHandler(void){


	BaseType_t xHigherPriorityTaskWoken = pdFALSE;


	if(EXTI->PR & EXTI_PR_PR1){

		EXTI->PR |= EXTI_PR_PR1;


		xSemaphoreGiveFromISR(ButtonCloseSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}

}


