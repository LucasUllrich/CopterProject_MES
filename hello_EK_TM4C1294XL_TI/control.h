/*
 * control.h
 *
 *  Created on: 08.01.2020
 *      Author: CCS_HATER_42
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#define PRINT_CTL_INPUT 0
#define PRINT_CTL_OUTPUT 0

#define NUM_OF_BUTTONS 5
#define NUM_OF_ADC_IN 5
#define STATUS_LED_INTERVAL 400 //ms
#define JOYSTICK_STEP_WIDTH 40  //uint16_t
#define JOYSTICK_DEAD_STEPS 2 //seen from idle position
#define ACCELERATOR_STEP_WIDTH 400 //uint16_t
/* because acceleration sensor does not use full ADC range, ACCELERATOR_STEP_WIDTH_MEASURED has to
 * be adapted after a change of ACCELERATOR_STEP_WIDTH to ensure useful values*/
#define ACCELERATOR_STEP_WIDTH_MEASURED 160
/* Measured idle value of acceleration sensor (horizontal position). Adaption after change of
 * ACCELERATOR_STEP_WIDTH required!*/
#define ACCELERATOR_IDLE_POS_MEASURED 200
#define ACCELERATOR_DEAD_STEPS 5//seen from idle position
#define ACCELERATOR_AXIS 3
#define ADC_RANGE 4096

#define VALUE_RANGE_MIN 1000
#define VALUE_RANGE_MAX 2000
#define VALUE_RANGE_ABS (VALUE_RANGE_MAX - VALUE_RANGE_MIN)

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

typedef struct Trim_Params {
    uint16_t yaw;
    uint16_t throttle;
    uint16_t trimSteps;
} Trim_Params;

void initControlHW();
uint32_t getValueFromADC(uint8_t adcPort);
void controlPoller (UArg *mailboxObject);

#endif /* INC_CONTROL_H_ */
