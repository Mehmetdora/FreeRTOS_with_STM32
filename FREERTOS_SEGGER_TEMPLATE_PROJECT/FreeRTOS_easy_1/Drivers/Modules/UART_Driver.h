/*
 * UART_Driver.h
 *
 *  Created on: Jul 3, 2026
 *      Author: mehmet_dora
 */

#ifndef MODULES_UART_DRIVER_H_
#define MODULES_UART_DRIVER_H_


#include "stdint.h"
#include "stm32f4xx.h"

void uart_init(void);

void uart_send_byte(void);
void uart_send_string(char[100]);




#endif /* MODULES_UART_DRIVER_H_ */
