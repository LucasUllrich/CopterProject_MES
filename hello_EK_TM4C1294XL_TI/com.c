/*
 * com.c
 *
 *  Created on: 06.01.2020
 *      Author: lullrich
 */

#include "com.h"


/************** Prototypes ***********************/
uint8_t _comUartSend (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count);


//void _uartSendWithRetry (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count)
//{
//    uint8_t counter;
//    uint8_t buf[UART_BUFFER_SIZE];
//
//    memcpy(buf, 0, UART_BUFFER_SIZE);
//
//    counter = 0;
//
//    do
//    {
//        comUartSendWithReceive (uart, buffer, count);
//        Task_sleep(30);
//        if (comUartReceive(uart, buf, UART_BUFFER_SIZE))
//        {
//            counter++;
//            continue;
//        }
//        break;
//    } while (counter < 5);
//}


UART_Handle _initUart (void)
{
    UART_Params uartParams;
    UART_Handle uart;

    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
//    uartParams.readCallback = comUartReceive;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.baudRate = 115200;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readTimeout = 5;

    uart = UART_open(Board_UART6, &uartParams);
    return uart;
}

uint8_t _initBTModule (UART_Handle uart)
{
    GPIO_write(TESTPIN, PIN_LOW);
    uint8_t counter;
    uint8_t buffer[UART_BUFFER_SIZE] = {0};

    GPIO_write(BT_RESET, PIN_LOW);
    Task_sleep(1);
    GPIO_write(BT_RESET, PIN_HIGH);
    Task_sleep(500);
    // Activate Bluetooth Module
    GPIO_write(BT_SW_BTN, PIN_HIGH);

    GPIO_write(BT_RTS, PIN_LOW);


    // Wait for status pins to signal for active BT Module
    while (!((GPIO_read(BT_STATUS1) == 0) && GPIO_read(BT_STATUS2) == 1));
    Task_sleep(100);

    // Receive to clear any status messages of BT Module
    if (comUartReceive(uart, buffer, UART_BUFFER_SIZE) == 0)
    {
        // BT Module send a %REBOOT% at startup, if nothing was received start again
        return 1;
    }

    // BT Module is ready, start configuration to assure correct behavior
    if (comUartSendWithReceive (uart, "$$$", strlen ("$$$")))
        return 1;

    // Switch to classic mode only
    if (comUartSendWithReceive (uart, "SG,2\r", strlen ("SG,2\r")))
        return 1;

    // Set security mode so no authentication is required
    if (comUartSendWithReceive (uart, "SA,2\r", strlen ("SA,2\r")))
        return 1;
    // pin required
//    comUartSend (uart, "SA,1\r", strlen ("SA,2\r"));

    // Set BT Module into pairing mode, it should establish the connection itself
//    comUartSend (uart, "SM,6\r", strlen ("SM,6\r"));
    if (comUartSendWithReceive (uart, "SM,0\r", strlen ("SM,0\r")))
        return 1;

    // Set BT Module into high power mode so it does not sleep
    if (comUartSendWithReceive (uart, "SH,1\r", strlen ("SH,1\r")))
        return 1;

    // Setup the BT Module to connect to the correct copter / phone
//    if (comUartSendWithReceive (uart, "SR,0006668CB270\r", strlen("SR,0006668CB270\r")))
//        return 1;

    // Reboot module
    if (comUartSendWithReceive (uart, "R,1\r", strlen ("R,1\r")))
        return 1;
    Task_sleep(2000);
    if (comUartReceive(uart, buffer, UART_BUFFER_SIZE) == 0)
    {
        // BT Module send a %REBOOT% at startup, if nothing was received start again
        return 1;
    }

    // Reenter command mode
    if (comUartSendWithReceive (uart, "$$$", strlen ("$$$")))
        return 1;

    // Connect explicitly to drone
//    counter = 0;
    if (_comUartSend (uart, "C,0006668CB270\r", strlen("C,0006668CB270\r")))
        return 1;


    // Wait until BT Module connected to drone
    while (1)
    {
        if (comUartReceive(uart, buffer, UART_BUFFER_SIZE) == 0)
            continue;
        if (strncmp((char *) buffer, "%RFCOMM_OPEN%", strlen("%RFCOMM_OPEN%")) == 0)
            break;
    }

    // End Setup
    if (_comUartSend (uart, "---", strlen ("---")))
        return 1;

    return 0;
}

uint8_t comUartSendWithReceive (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count)
{
    uint8_t buf[UART_BUFFER_SIZE];
    uint8_t index;
    uint8_t counter;

    counter = 0;

    memset(buf, 0, UART_BUFFER_SIZE);

    for (index = 0; index < count; index ++)
    {
        // wait until BT Module is ready
        while (GPIO_read(BT_CTS) == 1) {
            Task_sleep(1);
            counter ++;
            if (counter > 10) {
                return 1;
            }
        }
        // Write one Byte at a time to enable flow control
        UART_write(uart, &buffer[index], 1);
    }

    if (comUartReceive(uart, buf, UART_BUFFER_SIZE) == 0)
    {
        // BT Module send a %REBOOT% at startup, if nothing was received start again
        return 1;
    }
//    System_printf("Received: %s\n", *buf);
//    System_flush();
    return 0;
}


uint8_t _comUartSend (UART_Handle uart, uint8_t buffer[UART_BUFFER_SIZE], uint8_t count)
{
    uint8_t index;
    uint8_t counter;

    counter = 0;


    for (index = 0; index < count; index ++)
    {
        // wait until BT Module is ready
        while (GPIO_read(BT_CTS) == 1) {
            Task_sleep(1);
            counter ++;
            if (counter > 10) {
                return 1;
            }
        }
        // Write one Byte at a time to enable flow control
        UART_write(uart, &buffer[index], 1);
    }
    return 0;
}

/**
 * Try to read a UART response for 200 ms, use the short timeout to
 * achieve a fast response if something is read
 */
uint8_t comUartReceive (UART_Handle uart, void *buf, size_t count)
{
    uint8_t retval;
    uint8_t counter;
    counter = 0;
    retval = 0;
    GPIO_write(TESTPIN, PIN_HIGH);
    do {
        retval = UART_read(uart, buf, count);
        counter++;
        if (counter > 40) {
            GPIO_write(TESTPIN, PIN_LOW);
            return 0;
        }
    } while (retval == 0);

    GPIO_write(TESTPIN, PIN_LOW);
    return retval;
}

void comSender (UArg *mailboxObject, UArg arg1)
{
    Mailbox_Handle mailbox = (Mailbox_Handle) &mailboxObject;
    UART_Handle uart;
    Copter_Params copterParams;
//    uint16_t dummy;

    uart = _initUart();
    while(_initBTModule(uart));
    GPIO_write(Board_LED0, PIN_HIGH);


    while (1)
    {
        Mailbox_pend(mailbox, &copterParams, BIOS_NO_WAIT);
        System_printf("roll: %d, pitch: %d, yaw: %d, throttle: %d, arm: %d\n", copterParams.roll, copterParams.pitch, copterParams.yaw, copterParams.throttle, copterParams.arm);
        System_flush();
        Task_sleep (20);
    }
}
