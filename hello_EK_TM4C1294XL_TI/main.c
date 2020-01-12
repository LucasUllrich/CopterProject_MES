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
#include <xdc/runtime/Types.h>
#include <ti/sysbios/BIOS.h>



/* Example/Board Header files */


#include "main.h"
#include "com.h"
#include "control.h"


Copter_Params copterParams;

UART_Handle _initUart (void)
{
    UART_Params uartParams;
    UART_Handle uart;

    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.readCallback = comUartReceive;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.baudRate = 115200;
    uartParams.readEcho = UART_ECHO_OFF;

    uart = UART_open(Board_UART6, &uartParams);
    return uart;
}

void _initMailboxes (Mailbox_Handle *uartMailbox)
{
    Error_Block eb;
    Error_init(&eb);

    *uartMailbox = Mailbox_create(sizeof(Copter_Params), 1, NULL, &eb);
    if (Error_check(&eb))
    {
        // Stop system
        GPIO_write(Board_LED3, PIN_HIGH);
        while(1);
    }
}

//Clock_Struct clk0Struct, clk1Struct;
//Clock_Handle clk2Handle;
//Clock_Struct controlStruct;

/*
 *  ======== main ========
 */
Int main()
{
    UART_Handle uart;

    Mailbox_Handle uartMailbox;



    Types_FreqHz cpuFreq = {0};
    cpuFreq.lo = CPU_FREQ_LO;
    cpuFreq.hi = (CPU_FREQ_HI >> 32);
    BIOS_setCpuFreq (&cpuFreq);
    if (cpuFreq.lo != CPU_FREQ_LO)  // Omit hi value as 120 MHz fits in the lo value
    {
        // TODO: Add error led
        while(1);
    }

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    uart = _initUart();

//    System_printf("hello world\n");

    _initMailboxes(&uartMailbox);

    initControlHW();

    Error_Block eb;
    Error_init (&eb);

    Task_Handle controlTask;
    Task_Params controlParams;
    Task_Params_init(&controlParams);

    controlParams.arg0 = (UArg) (&uartMailbox);
    controlParams.arg1 = 0;
    controlParams.priority = 8;

    controlTask = Task_create((Task_FuncPtr)controlPoller, &controlParams, &eb);
    if (controlTask == NULL)
    {
        System_abort ("controlTask creation failed");
    }


    Task_Handle comTask;
    Task_Params comParams;
    Task_Params_init(&comParams);

    comParams.arg0 = (UArg) (&uartMailbox);
    comParams.arg1 = (UArg) (&uart);
    comParams.priority = 12;

    comTask = Task_create((Task_FuncPtr)comSender, &comParams, &eb);
    if (comTask == NULL)
    {
        System_abort ("comTask creation failed");
    }

    // TODO init of BT module needs an own task which is executed after BIOS_start()

    BIOS_start();
    return(0);
}
