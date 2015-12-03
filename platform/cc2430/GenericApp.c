/**************************************************************************************************
   Filename:       GenericApp.c
   Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
   Revision:       $Revision: 15793 $

   Description:    Generic Application (no Profile).


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

/*********************************************************************
   This application isn't intended to do anything useful, it is
   intended to be a simple example of an application's structure.

   This application sends "Hello World" to another "Generic"
   application every 15 seconds.  The application will also
   receive "Hello World" packets.

   The "Hello World" messages are sent/received as MSG type message.

   This applications doesn't have a profile, so it handles everything
   directly - itself.

   Key control:
    SW1:
    SW2:  initiates end device binding
    SW3:
    SW4:  initiates a match description request
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "GenericApp.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
#include "OnBoard.h"
#endif

/* HAL */
//#include "hal_lcd.h"
#include "hal_led.h"
//#include "hal_key.h"
//#include "hal_uart.h"

#include "Foo.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
//const cId_t GenericApp_ClusterList[PLATFORM_CC2430_GENERICAPP_MAX_CLUSTERS] =
//{
//  PLATFORM_CC2430_GENERICAPP_CLUSTERID
//};
/*
   const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
   {
   PLATFORM_CC2430_GENERICAPP_ENDPOINT,              //  int Endpoint;
   PLATFORM_CC2430_GENERICAPP_PROFID,                //  uint16 AppProfId[2];
   PLATFORM_CC2430_GENERICAPP_DEVICEID,              //  uint16 AppDeviceId[2];
   PLATFORM_CC2430_GENERICAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
   PLATFORM_CC2430_GENERICAPP_FLAGS,                 //  int   AppFlags:4;
   PLATFORM_CC2430_GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
   (cId_t *)GenericApp_ClusterList,  //  byte *pAppInClusterList;
   PLATFORM_CC2430_GENERICAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
   (cId_t *)GenericApp_ClusterList   //  byte *pAppInClusterList;
   };
 */
// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in GenericApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
//endPointDesc_t GenericApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte GenericApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // GenericApp_Init() is called.
devStates_t GenericApp_NwkState;

Foo data;
Foo* data_ptr = &data;
//byte GenericApp_TransID;  // This is the unique message ID (counter)

//afAddrType_t GenericApp_DstAddr;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
//void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
//void GenericApp_HandleKeys( byte shift, byte keys );
//void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void
GenericApp_SendTheMessage(void);
void
GenericApp_RecvTheMessage(void);
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      GenericApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void
GenericApp_Init(byte task_id) {
    GenericApp_TaskID = task_id;
    GenericApp_NwkState = DEV_INIT;
//  GenericApp_TransID = 0;
    DDS_init();
    // Device hardware initialization can be added here or in main() (Zmain.c).
    // If the hardware is application specific - add it here.
    // If the hardware is other parts of the device add it in main().

    //GenericApp_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
    // GenericApp_DstAddr.endPoint = 0;
    // GenericApp_DstAddr.addr.shortAddr = 0xffff;

    // Fill out the endpoint description.
    // GenericApp_epDesc.endPoint = PLATFORM_CC2430_GENERICAPP_ENDPOINT;
    // GenericApp_epDesc.task_id = &GenericApp_TaskID;
//  GenericApp_epDesc.simpleDesc
//           = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
// GenericApp_epDesc.latencyReq = noLatencyReqs;

    // Register the endpoint description with the AF
//  afRegister( &GenericApp_epDesc );

    // Register for all key events - This app will handle all key events
    RegisterForKeys(GenericApp_TaskID);

    // Update the display
#if defined ( LCD_SUPPORTED )
    HalLcdWriteString("GenericApp", HAL_LCD_LINE_1);
#endif
    // Start sending "the" message in a regular interval.
    osal_start_timerEx(GenericApp_TaskID,
                       PLATFORM_CC2430_GENERICAPP_SEND_MSG_EVT,
                       PLATFORM_CC2430_GENERICAPP_SEND_MSG_TIMEOUT);

//  ZDO_RegisterForZDOMsg( GenericApp_TaskID, End_Device_Bind_rsp );
//  ZDO_RegisterForZDOMsg( GenericApp_TaskID, Match_Desc_rsp );
}

/*********************************************************************
 * @fn      GenericApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
UINT16
GenericApp_ProcessEvent(byte task_id, UINT16 events) {
    afIncomingMSGPacket_t* MSGpkt;
    afDataConfirm_t* afDataConfirm;

    // Data Confirmation message fields
    byte sentEP;
    ZStatus_t sentStatus;
    byte sentTransID;     // This should match the value sent

    // Send a message out - This event is generated by a timer
    //  (setup in GenericApp_Init()).
    if ( events & PLATFORM_CC2430_GENERICAPP_SEND_MSG_EVT ) {
        // Send "the" message
#ifdef DATASOURCE
        GenericApp_SendTheMessage();
#endif
#ifdef DATASINK
        GenericApp_RecvTheMessage();
#endif
        // Setup to send message again
        osal_start_timerEx(GenericApp_TaskID,
                           PLATFORM_CC2430_GENERICAPP_SEND_MSG_EVT,
                           PLATFORM_CC2430_GENERICAPP_SEND_MSG_TIMEOUT);

        // return unprocessed events
        return (events ^ PLATFORM_CC2430_GENERICAPP_SEND_MSG_EVT);
    }

    // Discard unknown events
    return 0;
}

int
getTemp(void) {
    unsigned int value;
    SAMPLE_TEMP_SENSOR(value);
    return ADC14_TO_CELSIUS(value);
    // return ADC14_TO_FARENHEIT(value);
}


/*********************************************************************
 * @fn      GenericApp_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
void
GenericApp_SendTheMessage(void) {
    // char theMessageData[] = "Hello World";

    int temper = getTemp();
    data_ptr->temperatur = (int) (temper * 10);
    // data_ptr->temperatur = 10;
    data_ptr->id = GenericApp_TaskID;
    DDS_FooDataWriter_write(fooDataWriter, &data, NULL);

    /*
       if ( AF_DataRequest( &GenericApp_DstAddr, &GenericApp_epDesc,
                         PLATFORM_CC2430_GENERICAPP_CLUSTERID,
                         (byte)osal_strlen( theMessageData ) + 1,
                         (byte *)&theMessageData,
                         &GenericApp_TransID,
                         AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS
                            )
       {
       // Successfully requested to be sent.
       }
       else
       {
       // Error occurred in request to send.
       }
     */

}

void
GenericApp_RecvTheMessage(void) {
    Foo d;
    Foo* d_ptr = &d;

    DDS_ReturnCode_t ret;
    ret = DDS_FooDataReader_take_next_sample(fooDataReader, &d_ptr, NULL);

    if (ret == DDS_RETCODE_OK) {
        HalLedSet(HAL_LED_1, HAL_LED_MODE_TOGGLE);
    }
}

/*********************************************************************
*********************************************************************/