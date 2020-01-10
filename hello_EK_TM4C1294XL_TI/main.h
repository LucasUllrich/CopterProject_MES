/*
 * main.h
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <string.h>

/* BIOS Module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
//#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/runtime/Error.h>

#include "Board.h"

// Defines
#define COPTER_MAC  ("0006668CB270")
#define UART_BUFFER_SIZE (32)
#define CPU_FREQ 120000000
#define CPU_FREQ_LO (CPU_FREQ & 0xffffffff)
#define CPU_FREQ_HI ((CPU_FREQ & 0xffffffff00000000) >> 32)
#define US_TO_CLOCKTICKS(time) (((float)time / 1000000) / CPU_FREQ)

//extern Mailbox_Handle uartMailbox;

typedef struct Copter_Params {
    uint16_t roll;
    uint16_t pitch;
    uint16_t yaw;
    uint16_t throttle;
    uint8_t arm;
} Copter_Params;


#endif /* MAIN_H_ */