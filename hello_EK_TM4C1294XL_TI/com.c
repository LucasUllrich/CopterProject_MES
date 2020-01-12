/*
 * com.c
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#include "com.h"


void _initBTModule (UART_Handle uart)
{
    // Activate Bluetooth Module
    GPIO_write(BT_SW_BTN, PIN_HIGH);

    // Wait for status pins to signal for active BT Module
//    while (!((GPIO_read(BT_STATUS1) == 0) && GPIO_read(BT_STATUS2) == 1));
    while (1)
    {
        uint8_t btStatus1;
        uint8_t btStatus2;
        uint8_t btReset;

        btReset = GPIO_read(BT_RESET);
        btStatus1 = GPIO_read(BT_STATUS1);
        btStatus2 = GPIO_read(BT_STATUS2);
    }

    // BT Module is ready, start configuration to assure correct behavior
    comUartSend (uart, "$$$", strlen ("$$$"));

    // Set security mode so no authentication is required
    comUartSend (uart, "SA,2", strlen ("SA,2"));

    // Set BT Module into pairing mode, it should establish the connection itself
    comUartSend (uart, "SM,6", strlen ("SM,6"));

    // Switch to classic mode only
    comUartSend (uart, "SG,2", strlen ("SG,2"));

    // Set BT Module into high power mode so it does not sleep
    comUartSend (uart, "SH,1", strlen ("SH,1"));

    // Setup the BT Module to connect to the correct copter
    uint8_t buffer[15] = "SR,";
    strcat ((char*) buffer, COPTER_MAC);
    comUartSend (uart, buffer, 3 + 12); // TODO Check content of buffer

    // Check connection status
    comUartSend (uart, "GK", strlen ("GK"));

    // End Setup
    comUartSend (uart, "---", strlen ("---"));
}

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

void comSender (UArg *mailboxObject, UArg *uartObject)
{
    Mailbox_Handle mailbox = (Mailbox_Handle) &mailboxObject;
    UART_Handle uart = (UART_Handle) &uartObject;
    Copter_Params copterParams;
    uint16_t dummy;

    _initBTModule(uart);

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
