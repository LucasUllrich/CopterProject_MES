/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main.c ========
 */

/* XDC Module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "Board.h"

#include "inc/main.h"


UART_Handle _initUart (void)
{
    UART_Params uartParams;
    UART_Handle uart;

    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.readCallback = ComUartReceive;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.baudRate = 115200;
    uartParams.readEcho = UART_ECHO_OFF;

    uart = UART_open(Board_UART6, &uartParams);
    return uart;
}


void _initBTModule (UART_Handle uart)
{
    // Activate Bluetooth Module
    GPIO_write(BT_SW_BTN, PIN_HIGH);

    // Wait for status pins to signal for active BT Module
    while (!((GPIO_read(BT_STATUS1) == 0) && GPIO_read(BT_STATUS2) == 1));

    // BT Module is ready, start configuration to assure correct behavior
    ComUartSend (uart, "$$$", strlen ("$$$"));

    // Set security mode so no authentication is required
    ComUartSend (uart, "SA,2", strlen ("SA,2"));

    // Set BT Module into pairing mode, it should establish the connection itself
    ComUartSend (uart, "SM,6", strlen ("SM,6"));

    // Switch to classic mode only
    ComUartSend (uart, "SG,2", strlen ("SG,2"));

    // Set BT Module into high power mode so it does not sleep
    ComUartSend (uart, "SH,1", strlen ("SH,1"));

    // Setup the BT Module to connect to the correct copter
    uint8_t buffer[15] = "SR", strlen ("SR,1"));
    strcat ((char*) buffer, COPTER_MAC);
    ComUartSend (uart, buffer); // TODO Check content of buffer

    // Check connection status
    ComUartSend (uart, "GK", strlen ("GK"));

    // End Setup
    ComUartSend (uart, "---");
}

void _initMailboxes (void)
{
    Error_Block eb;
    Error_init(&eb);

    uartMailbox = Mailbox_create(SEND_BUFFER_SIZE, 1, NULL, &eb);
    if (Error_check(&eb))
    {
        // Stop system
        GPIO_write(Board_LED3, PIN_HIGH);
        while(1);
    }
}


/*
 *  ======== main ========
 */
Int main()
{
    UART_Handle uart;

    /* Call board init functions */
    Board_initGeneral();
    Board_initUART();

    uart = _initUart();
    _initBTModule(uart);

    System_printf("hello world\n");


    Mailbox_params_init();

    // TODO init of BT module needs an own task which is executed after BIOS_start()

    BIOS_start();
    return(0);
}
