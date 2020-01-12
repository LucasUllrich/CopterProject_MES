/*
 * com.c
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#include "com.h"


void comUartSend (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count)
{
    uint8_t index;
    uint8_t lineEndChar = 0x0d;
    for (index = 0; index < count; index ++)
    {
        // wait until BT Module is ready
        while (GPIO_read(BT_CTS) == 1);
        // Write one Byte at a time to enable flow control
//        sendDummy = buffer[index]
        UART_write(uart, &buffer[index], 1);
    }

    while (GPIO_read(BT_CTS) == 1);
    UART_write (uart, &lineEndChar, 1);
}

void comUartReceive (UART_Handle uart, void *buf, size_t count)
{

}

void comSender (UArg *mailboxObject, UArg arg1)
{
    Mailbox_Handle mailbox = (Mailbox_Handle) &mailboxObject;
    Copter_Params copterParams;
    uint16_t dummy;
    while (1)
    {
        Mailbox_pend(mailbox, &copterParams, BIOS_NO_WAIT);
        dummy = copterParams.roll;
//        System_printf("comTask\n");
        System_printf("comTask: %d\n", dummy);
        System_flush();
        Task_sleep (20);
    }
}
