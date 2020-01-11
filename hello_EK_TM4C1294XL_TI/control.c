/*
 * control.c
 *
 *  Created on: 08.01.2020
 *      Author: lullrich
 */

#include "control.h"

void controlPoller (UArg mailboxObject)
{
    // TODO: Please check for correctness
//    Mailbox_Handle mailbox = (Mailbox_Handle) mailboxObject;
    Copter_Params copterParams;
    copterParams.roll = 10;
//    Mailbox_post(mailbox, &copterParams, BIOS_NO_WAIT);
}
