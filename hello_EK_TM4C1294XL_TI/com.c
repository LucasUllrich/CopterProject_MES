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
    for (index = 0; index < count; index ++)
    {
        // wait until BT Module is ready
        while (GPIO_read(BT_CTS) == 1);
        // Write one Byte at a time to enable flow control
//        UART_write(uart, buffer[index], 1);
    }

    while (GPIO_read(BT_CTS) == 1);
//    UART_write (uart, 0x0d, 1); // Send line ending character
}

void comUartReceive (UART_Handle uart, void *buf, size_t count)
{

}

void comSender (xdc_UArg mailboxObject)
{
//    Mailbox_Handle mailbox = (Mailbox_Handle) mailboxObject;
    Copter_Params copterParams = {0};
//    Mailbox_pend(mailbox, &copterParams, BIOS_NO_WAIT);
    uint16_t dummy = copterParams.roll;
}
