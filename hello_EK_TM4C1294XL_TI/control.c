/*
 * control.c
 *
 *  Created on: 08.01.2020
 *      Author: CCS_HATER_42
 */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include "control.h"

void controlPoller (UArg mailboxObject)
{
    // TODO: Please check for correctness
    Mailbox_Handle mailbox = (Mailbox_Handle) mailboxObject;
    Copter_Params copterParams;
    int32_t usrButton1 = 0;
    int32_t usrButton2 = 0;
    int32_t bordButton1 = 0;
    int32_t bordButton2 = 0;
    int32_t armButton = 0;
    int32_t analogInputs[5];
    uint8_t analogPins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};
    uint16_t statusLedCounter = 0;
    bool statusLedStatus = false;
    uint8_t i = 0;

    initControlHW();
    memset(analogInputs, 0, sizeof(analogInputs));

    copterParams.roll = 10;
    //Mailbox_post(mailbox, &copterParams, BIOS_NO_WAIT); //does not work so far...

#if PRINT_CTL_INPUT
    System_printf("Starting to poll controller input...\n");
    System_flush();
#endif

    copterParams.arm = false;
    while(1){
        usrButton1 = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1);
        usrButton2 = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2);
        bordButton1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
        bordButton2 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1);
        armButton = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
        for(i = 0; i < sizeof(analogInputs); i++){
            analogInputs[i] = GPIOPinRead(GPIO_PORTE_BASE, analogPins[i]);
        }
#if PRINT_CTL_INPUT
        System_printf("Yaw trim: B1 = %d, B2 = %d, Throttle trim: B1 = %d, B2 = %d, Arm = %d\n", usrButton1, usrButton2, bordButton1, bordButton2, armButton);
        System_printf("Acceleration: X = %d, Y = %d, Z = %d, Joystick: Y = %d, X = %d\n", analogInputs[0], analogInputs[1], analogInputs[2], analogInputs[3], analogInputs[4]);
        System_flush();
#endif
        statusLedCounter++;
        if(statusLedCounter >= (uint16_t)(STATUS_LED_INTERVAL/20)){
            if(statusLedStatus){
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
                statusLedStatus = false;
            }
            else{
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
                statusLedStatus = true;
            }
            statusLedCounter = 0;
        }
    }
}

void initControlHW(){
    // Enable the GPIO ports for status LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1))
    {
    }

    // Enable the GPIO ports.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ) ||
            !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }

    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
}
