/*
 * com.h
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#ifndef INC_COM_H_
#define INC_COM_H_

#include <ti/drivers/UART.h>

#include "main.h"


void comUartSend (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count);
void comUartReceive (UART_Handle uart, void *buf, size_t count);
void comSender (xdc_UArg mailboxObject);


#endif /* INC_COM_H_ */
