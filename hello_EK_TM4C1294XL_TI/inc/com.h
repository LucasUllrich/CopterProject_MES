/*
 * com.h
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#ifndef INC_COM_H_
#define INC_COM_H_

#include <ti/drivers/UART.h>

#include "inc/main.h"


void ComUartSend (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count);
UART_Callback ComUartReceive (UART_Handle uart, void *buf, size_t count);


#endif /* INC_COM_H_ */
