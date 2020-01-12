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

void assignButtonState(Copter_Params* paramStruc, Trim_Params* trimParams, int32_t debouncedButtonVal, uint8_t buttonIdentifier){
    switch(buttonIdentifier){
    //yaw trim to right
    case 0:{
        if(debouncedButtonVal == 0){
            if(paramStruc->arm > 0){
                if((trimParams->yaw + trimParams->trimSteps) <= VALUE_RANGE_MAX){
                    trimParams->yaw = trimParams->yaw + trimParams->trimSteps;
                }
            }
            else {
                trimParams->trimSteps = (trimParams->trimSteps == 1)?10:1;
            }
        }
    }break;
    //yaw trim to left
    case 1:{
        if(debouncedButtonVal == 0){
            if(paramStruc->arm > 0){
                if((trimParams->yaw + trimParams->trimSteps) >= VALUE_RANGE_MIN){
                    trimParams->yaw = trimParams->yaw - trimParams->trimSteps;
                }
            }
        }
    }break;
    //throttle trim up
    case 2:{
        if(debouncedButtonVal == 0){
            if((trimParams->throttle + trimParams->trimSteps) <= VALUE_RANGE_MAX){
                trimParams->throttle = trimParams->throttle + trimParams->trimSteps;
            }
        }
    }break;
    //throttle trim down
    case 3:{
        if(debouncedButtonVal == 0){
            if((trimParams->throttle + trimParams->trimSteps) >= VALUE_RANGE_MIN){
                trimParams->throttle = trimParams->throttle - trimParams->trimSteps;
            }
        }
    }break;
    //arm state
    case 4:{
        if(debouncedButtonVal == 0){
            (paramStruc->arm == 0)?(paramStruc->arm = 1):(paramStruc->arm = 0);
        }
    }break;
    default:{}break;
    }
}

uint16_t calculateOutputValueForJoystick(uint16_t trimParam, uint32_t readVal){
    uint16_t retVal = 1500;
    uint16_t offsetToZero = VALUE_RANGE_MIN;

    if(readVal > round(JOYSTICK_STEP_WIDTH/2 + JOYSTICK_DEAD_STEPS)){
        if(trimParam > (VALUE_RANGE_MIN + round(VALUE_RANGE_ABS/2))){
            offsetToZero = VALUE_RANGE_MIN + (trimParam - (VALUE_RANGE_MIN + round(VALUE_RANGE_ABS/2)))*2;
        }
        else if(trimParam < (VALUE_RANGE_MIN + round(VALUE_RANGE_ABS/2))){
            offsetToZero = VALUE_RANGE_MIN - ((VALUE_RANGE_MIN + round(VALUE_RANGE_ABS/2)) - trimParam)*2;
        }
        retVal = (uint16_t)(readVal * round((VALUE_RANGE_MAX - trimParam)/round(JOYSTICK_STEP_WIDTH/2)) + offsetToZero);
    }
    else if(readVal < round(JOYSTICK_STEP_WIDTH/2 - JOYSTICK_DEAD_STEPS)){
        retVal = (uint16_t)(readVal * round((trimParam - VALUE_RANGE_MIN)/round(JOYSTICK_STEP_WIDTH/2)) + VALUE_RANGE_MIN);
    }
    else{
        retVal = trimParam;
    }

    if(retVal > VALUE_RANGE_MAX){retVal = VALUE_RANGE_MAX;}
    if(retVal < VALUE_RANGE_MIN){retVal = VALUE_RANGE_MIN;}

    return retVal;
}

uint16_t calculateOutputValueForAccelerator(uint32_t readVal){
    uint16_t retVal = 1500;

    if(readVal < (ACCELERATOR_IDLE_POS_MEASURED - round(ACCELERATOR_STEP_WIDTH_MEASURED/2))){
        readVal = 0;
    }
    else {
        readVal = readVal - (ACCELERATOR_IDLE_POS_MEASURED - round(ACCELERATOR_STEP_WIDTH_MEASURED/2));
    }

    if(readVal < (ACCELERATOR_IDLE_POS_MEASURED - ACCELERATOR_DEAD_STEPS) || readVal > (ACCELERATOR_IDLE_POS_MEASURED + ACCELERATOR_DEAD_STEPS)){
        retVal = (uint16_t)((readVal * round(VALUE_RANGE_MIN/ACCELERATOR_STEP_WIDTH_MEASURED)) + VALUE_RANGE_MIN);
    }

    if(retVal > VALUE_RANGE_MAX){retVal = VALUE_RANGE_MAX;}
    if(retVal < VALUE_RANGE_MIN){retVal = VALUE_RANGE_MIN;}

    return retVal;
}

void controlPoller(UArg *mailboxObject)
{
#ifndef DEACTIVATE_CONTROLLER
    // TODO: Please check for correctness
//    Mailbox_Handle mailbox = (Mailbox_Handle) &mailboxObject;
    Copter_Params copterParams;
    Trim_Params copterTrim;
    int32_t buttonValues[NUM_OF_BUTTONS] = {0};
    uint32_t analogInputs[NUM_OF_ADC_IN] = {0};
    uint32_t adcStepWidth = 1;
    uint16_t statusLedCounter = 0;
    uint8_t analogPins[] = {ADC_CTL_CH3, ADC_CTL_CH2, ADC_CTL_CH1, ADC_CTL_CH9, ADC_CTL_CH0};
    bool statusLedStatus = false;
    uint8_t i = 0;

    int32_t oldButtonValues[NUM_OF_BUTTONS] = {0};
    int32_t validButtonValues[NUM_OF_BUTTONS] = {0};
    uint32_t validADCValues[NUM_OF_ADC_IN] = {0};

    //init copter params
    copterParams.yaw = 1500;
    copterParams.throttle = 0;
    copterParams.pitch = 1500;
    copterParams.roll = 1500;
    copterParams.arm = 0;

    //init copter trim params
    copterTrim.yaw = 1500;
    copterTrim.throttle = 1400;
    copterTrim.trimSteps = 1;

    //Mailbox_post(mailbox, &copterParams, BIOS_NO_WAIT); //does not work so far...

#if PRINT_CTL_INPUT
    System_printf("Starting to poll controller input...\n");
    System_flush();
#endif

#endif // DEACTIVATE_CONTROLLER

    while(1){
#ifndef DEACTIVATE_CONTROLLER
        //read inputs
        // TODO: use the RTOS function GPIO_read instead of the tiva function!!
        buttonValues[0] = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1);
        buttonValues[1] = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2);
        buttonValues[3] = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
        buttonValues[2] = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1);
        buttonValues[4] = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6);
        for(i = 0; i < NUM_OF_ADC_IN; i++){
            analogInputs[i] = getValueFromADC(analogPins[i]);
            if(i < ACCELERATOR_AXIS){adcStepWidth = ACCELERATOR_STEP_WIDTH;}
            else{adcStepWidth = JOYSTICK_STEP_WIDTH;}
            validADCValues[i] = (uint32_t)round(analogInputs[i]/round(ADC_RANGE/adcStepWidth));
        }

        //Debouncing buttons
        for(i = 0; i < NUM_OF_BUTTONS; i++){
            if(buttonValues[i] != oldButtonValues[i]){
                oldButtonValues[i] = buttonValues[i];
            }
            if(buttonValues[i] != validButtonValues[i] && buttonValues[i] == oldButtonValues[i]){
                validButtonValues[i] = buttonValues[i];
                assignButtonState(&copterParams, &copterTrim, validButtonValues[i], i);
            }
        }

        copterParams.yaw = calculateOutputValueForJoystick(copterTrim.yaw, validADCValues[3]);
        copterParams.throttle = calculateOutputValueForJoystick(copterTrim.throttle, validADCValues[4]);
        copterParams.pitch = calculateOutputValueForAccelerator(validADCValues[1]);
        copterParams.roll = calculateOutputValueForAccelerator(validADCValues[0]);

#if PRINT_CTL_INPUT
        System_printf("Yaw trim: B1 = %d, B2 = %d, Throttle trim: B1 = %d, B2 = %d, Arm = %d\n", buttonValues[0], buttonValues[1], buttonValues[2], buttonValues[3], buttonValues[4]);
        System_printf("Acceleration: X = %d, Y = %d, Z = %d, Joystick: X = %d, Y = %d\n", validADCValues[0], validADCValues[1], validADCValues[2], validADCValues[3], validADCValues[4]);
        System_flush();
#endif
#if PRINT_CTL_OUTPUT
        System_printf("Arm = %d, Yaw = %d, Throttle = %d, Pitch = %d, Roll = %d\n", copterParams.arm, copterParams.yaw, copterParams.throttle, copterParams.pitch, copterParams.roll);
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
