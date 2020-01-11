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
    uint32_t analogInputs[5];
    uint8_t analogPins[] = {ADC_CTL_CH3, ADC_CTL_CH2, ADC_CTL_CH1, ADC_CTL_CH9, ADC_CTL_CH0};
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
        for(i = 0; i < sizeof(analogPins); i++){
            analogInputs[i] = getValueFromADC(analogPins[i]);
        }
#if PRINT_CTL_INPUT
        System_printf("Yaw trim: B1 = %d, B2 = %d, Throttle trim: B1 = %d, B2 = %d, Arm = %d\n", usrButton1, usrButton2, bordButton1, bordButton2, armButton);
        System_printf("Acceleration: X = %d, Y = %d, Z = %d, Joystick: X = %d, Y = %d\n", analogInputs[0], analogInputs[1], analogInputs[2], analogInputs[3], analogInputs[4]);
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
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0) /*|| !SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1)*/)
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

    // Set each of the user button GPIO pins as an input with a pull-up.
    GPIODirModeSet(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Set each of the board button GPIO pins as an input with a pull-up.
    GPIODirModeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Set the joystick button GPIO pin as an input with a pull-up.
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
}

uint32_t getValueFromADC(uint8_t adcPort){
    uint32_t readVal[1];

    memset(readVal, 0, sizeof(readVal));

    //disabling ADC for configuration
    ADCSequenceDisable(ADC0_BASE, 3);
    //reconfigure ADC with new port (just switching a multiplexer in an inconvenient way because there is no different function for that...)
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, adcPort | ADC_CTL_IE | ADC_CTL_END);
    //enabling ADC with same configuration as before, just reading on different input now...
    ADCSequenceEnable(ADC0_BASE, 3);
    // Clear the ADC interrupt flag after configuration / before waiting for value.
    ADCIntClear(ADC0_BASE, 3);
    // Trigger the ADC conversion.
    ADCProcessorTrigger(ADC0_BASE, 3);
    // Wait for conversion to be completed.
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }
    // Clear the ADC interrupt flag.
    ADCIntClear(ADC0_BASE, 3);
    // Read ADC Value.
    ADCSequenceDataGet(ADC0_BASE, 3, readVal);

    return readVal[0];
}
