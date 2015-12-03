/**************************************************************************************************
   Filename:       GenericApp.h
   Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
   Revision:       $Revision: 15793 $

   Description:    This file contains the Generic Application definitions.


   Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

   IMPORTANT: Your use of this Software is limited to those specific rights
   granted under the terms of a software license agreement between the user
   who downloaded the software, his/her employer (which must be your employer)
   and Texas Instruments Incorporated (the "License").  You may not use this
   Software unless you agree to abide by the terms of the License. The License
   limits your use, and you acknowledge, that the Software may not be modified,
   copied or distributed unless embedded on a Texas Instruments microcontroller
   or used solely and exclusively in conjunction with a Texas Instruments radio
   frequency transceiver, which is integrated into your product.  Other than for
   the foregoing purpose, you may not use, reproduce, copy, prepare derivative
   works of, modify, distribute, perform, display or sell this Software and/or
   its documentation for any purpose.

   YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
   PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
   INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
   NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
   TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
   NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
   LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
   INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
   OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
   OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
   (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

   Should you have any questions regarding your right to use this Software,
   contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef GENERICAPP_H
#define GENERICAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
//#ifdef DATASOURCE
#define PLATFORM_CC2430_GENERICAPP_ENDPOINT           10
//#endif
//#ifdef DATASINK
//#define PLATFORM_CC2430_GENERICAPP_ENDPOINT           11
//#endif

#define PLATFORM_CC2430_GENERICAPP_PROFID             0x0F04
#define PLATFORM_CC2430_GENERICAPP_DEVICEID           0x0001
#define PLATFORM_CC2430_GENERICAPP_DEVICE_VERSION     0
#define PLATFORM_CC2430_GENERICAPP_FLAGS              0

#define PLATFORM_CC2430_GENERICAPP_MAX_CLUSTERS       1
#define PLATFORM_CC2430_GENERICAPP_CLUSTERID          1

// Send Message Timeout
#define PLATFORM_CC2430_GENERICAPP_SEND_MSG_TIMEOUT   5000     // Every 5
                                                               // seconds

// Application Events (OSAL) - These are bit weighted definitions.
#define PLATFORM_CC2430_GENERICAPP_SEND_MSG_EVT       0x0001

/*********************************************************************
 * MACROS
 */

#include "iocc2430.h"


int
getTemp(void);
/*********************************************************************
 * MACROS
 */

#define ADC14_TO_CELSIUS(ADC_VALUE) \
    ( ((int)ADC_VALUE * (int)GRADIENT_C) /100000 + OFFSET_C )

#define ADC14_TO_FARENHEIT(ADC_VALUE) \
    ( (int)ADC_VALUE * (int)GRADIENT_F + OFFSET_F )

#define SAMPLE_TEMP_SENSOR(v)      \
    do {                                                      \
        ADCCON2 = 0x3E;                          \
        ADCCON1 = 0x73;                           \
        while(!(ADCCON1 & 0x80)) ;              \
        v = ADCL;                                       \
        v |= ( ( (unsigned int)ADCH) << 8 );    \
    } while (0)

/*********************************************************************
 * TYPEDEFS
 */

/* OFFSET_C must be calculated/measured for each individual chip */
#define GRADIENT_C    1496
#define GRADIENT_F    (9.0/5.0 * GRADIENT_C)
#define OFFSET_C      -296
#define OFFSET_F      (9.0/5.0 * OFFSET_C) + 32


/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Generic Application
 */
extern void
GenericApp_Init(byte task_id);
extern void
ZigBee_init(byte task_id);

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16
GenericApp_ProcessEvent(byte task_id, UINT16 events);
extern UINT16
ZigBee_ProcessEvent(byte task_id, UINT16 events);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GENERICAPP_H */