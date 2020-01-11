/*
 * control.h
 *
 *  Created on: 08.01.2020
 *      Author: lullrich
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#define PRINT_CTL_INPUT 1
#define PRINT_CTL_OUTPUT 1
#define STATUS_LED_INTERVAL 400 //ms
#define JOYSTICK_STEP_WIDTH 40  //uint16_t
#define ACCELERATOR_STEP_WIDTH 200 //uint16_t
#define ACCELERATOR_AXIS 3
#define ADC_RANGE 4096

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

void initControlHW();
uint32_t getValueFromADC(uint8_t adcPort);
void controlPoller (UArg *mailboxObject);

#endif /* INC_CONTROL_H_ */
