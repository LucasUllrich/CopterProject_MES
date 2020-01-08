/*
 * main.h
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <string.h>
#include <ti/sysbios/knl/Mailbox.h>

#include "inc/com.h"

// Defines
#define COPTER_MAC  ("0006668CB270")
#define UART_BUFFER_SIZE (32)

extern Mailbox_Handle uartMailbox;


#endif /* MAIN_H_ */
