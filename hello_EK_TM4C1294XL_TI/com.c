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
//    uint8_t counter;
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

void copterSend (UART_Handle uart, Copter_Params copterParams)
{
    uint8_t buffer[UART_BUFFER_SIZE];
    uint8_t checksum;
    uint8_t counter;
    static uint8_t armState = 0;

    buffer[0] = (uint8_t) 0x24;
    buffer[1] = (uint8_t) 0x4D;
    buffer[2] = (uint8_t) 0x3C;
    buffer[3] = (uint8_t) 0x0A;
    buffer[4] = (uint8_t) 0xC8;

//    buffer[5] = (uint8_t) (copterParams.pitch & 0x00FF);
//    buffer[6] = (uint8_t) ((copterParams.pitch >> 8) & 0x00FF);
//    buffer[7] = (uint8_t) (copterParams.roll & 0x00FF);
//    buffer[8] = (uint8_t) ((copterParams.roll >> 8) & 0x00FF);

    buffer[5] = (uint8_t) (1500 & 0x00FF);
    buffer[6] = (uint8_t) ((1500 >> 8) & 0x00FF);
    buffer[7] = (uint8_t) (1500 & 0x00FF);
    buffer[8] = (uint8_t) ((1500 >> 8) & 0x00FF);

    buffer[9] = (uint8_t) (copterParams.throttle & 0x00FF);
    buffer[10] = (uint8_t) ((copterParams.throttle >> 8) & 0x00FF);
    buffer[11] = (uint8_t) (copterParams.yaw & 0x00FF);
    buffer[12] = (uint8_t) ((copterParams.yaw >> 8) & 0x00FF);

    if (copterParams.arm == 0x01) {
        buffer[13] = (uint8_t) 0xd0;
        buffer[14] = (uint8_t) 0x07;
        System_printf("Armed\n");
        // Send the first packet with minimal throttle
        if (armState == 0)
        {
            System_printf("Reset throttle\n");
            buffer[9] = (uint8_t) (1000 & 0x00FF);
            buffer[10] = (uint8_t) ((1000 >> 8) & 0x00FF);
        }
        System_flush();
    } else {
        armState = 0;
        buffer[9] = (uint8_t) (1000 & 0x00FF);
        buffer[10] = (uint8_t) ((1000 >> 8) & 0x00FF);

        buffer[13] = (uint8_t) 0xe8;
        buffer[14] = (uint8_t) 0x03;
    }

    checksum = 0;

    for (counter = 3; counter < PACKET_SIZE - 1; counter++)
        checksum ^= buffer[counter];

    buffer[15] = checksum;


    _comUartSend (uart, buffer, PACKET_SIZE);
    if (copterParams.arm == 0x01)
    {
        armState = 1;
    }
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

void comSender (UArg arg0, UArg arg1) //(UArg *mailboxObject, UArg arg1)
{
//    Mailbox_Handle mailbox = (Mailbox_Handle) &mailboxObject;
    UART_Handle uart;
    Copter_Params copterParams;
//    uint16_t dummy;

    uart = _initUart();
//    while(_initBTModule(uart));
    GPIO_write(Board_LED0, PIN_HIGH);


    while (1)
    {
        Mailbox_pend(mailbox, &copterParams, BIOS_NO_WAIT);
//        System_printf("roll: %d, pitch: %d, yaw: %d, throttle: %d, arm: %d\n", copterParams.roll, copterParams.pitch, copterParams.yaw, copterParams.throttle, copterParams.arm);
//        System_flush();
        copterSend(uart, copterParams);
        Task_sleep (20);
    }
}
