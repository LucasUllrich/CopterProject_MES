/*
 * control.c
 *
 *  Created on: 08.01.2020
 *      Author: CCS_HATER_42
 */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <math.h>

#include "control.h"

void controlPoller (UArg *mailboxObject)
{
#ifndef DEACTIVATE_CONTROLLER
    // TODO: Please check for correctness
//    Mailbox_Handle mailbox = (Mailbox_Handle) &mailboxObject;
    Copter_Params copterParams;
    int32_t usrButton1 = 0;
    int32_t usrButton2 = 0;
    int32_t bordButton1 = 0;
    int32_t bordButton2 = 0;
    int32_t armButton = 0;
    uint32_t analogInputs[5] = {0};
    uint32_t adcStepWidth = 1;
    uint16_t statusLedCounter = 0;
    uint8_t analogPins[] = {ADC_CTL_CH3, ADC_CTL_CH2, ADC_CTL_CH1, ADC_CTL_CH9, ADC_CTL_CH0};
    bool statusLedStatus = false;
    uint8_t i = 0;

    // TODO: static ist hier jetzt nicht mehr notwendig, ist ja jetzt ein task
    static int32_t oldButtonValues[5] = {0};
    static int32_t validButtonValues[5] = {0};
    static uint32_t validADCValues[5] = {0};

    copterParams.roll = 10;

    //Mailbox_post(mailbox, &copterParams, BIOS_NO_WAIT); //does not work so far...

#if PRINT_CTL_INPUT
    System_printf("Starting to poll controller input...\n");
    System_flush();
#endif

    copterParams.arm = false;
#endif // DEACTIVATE_CONTROLLER

    while(1){
        // TODO: lösch mich wenn du mich nicht mehr brauchst
        System_printf("ControlTask\n");
        System_flush();
#ifndef DEACTIVATE_CONTROLLER
        //read inputs
        // TODO: use the RTOS function GPIO_read instead of the tiva function!!
        usrButton1 = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1);
        usrButton2 = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2);
        bordButton2 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
        bordButton1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1);
        armButton = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
        for(i = 0; i < sizeof(analogPins); i++){
            analogInputs[i] = getValueFromADC(analogPins[i]);
            if(i < ACCELERATOR_AXIS){adcStepWidth = ACCELERATOR_STEP_WIDTH;}
            else{adcStepWidth = JOYSTICK_STEP_WIDTH;}
            validADCValues[i] = (uint32_t)round(analogInputs[i]/round(ADC_RANGE/adcStepWidth));
        }

        if(usrButton1 != oldButtonValues[0]){

        }
#if PRINT_CTL_INPUT
        System_printf("Yaw trim: B1 = %d, B2 = %d, Throttle trim: B1 = %d, B2 = %d, Arm = %d\n", usrButton1, usrButton2, bordButton1, bordButton2, armButton);
        System_printf("Acceleration: X = %d, Y = %d, Z = %d, Joystick: X = %d, Y = %d\n", validADCValues[0], validADCValues[1], validADCValues[2], validADCValues[3], validADCValues[4]);
        System_flush();
#endif
        // handle status LED
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
#endif // DEACTIVATE_CONTROLLER
        Task_sleep(20);
    }
}

void initControlHW(){
#ifndef DEACTIVATE_CONTROLLER
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
#endif
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
