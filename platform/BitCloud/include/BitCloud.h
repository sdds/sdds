/*
 * BitCloud.h
 *
 *  Created on: Aug 9, 2010
 *      Author: kai
 */

#ifndef BITCLOUD_H_
#define BITCLOUD_H_

#include <macAddr.h>
#include <appFramework.h>
#include <configServer.h>
#include <appTimer.h>
#include <aps.h>
#include <uid.h>
#include <appTimer.h>
#include <zdo.h>
#include <dbg.h>

//#include <types.h>
// application state
typedef enum
{
  ZIGBEE_STATE_INITIAL,           // Zigbee device initial state (after Power On or Reset)
  ZIGBEE_STATE_START_WAIT,        // Waiting while the Button0 was not pressed
  ZIGBEE_STATE_NETWORK_JOINING,   // Joining network state
  ZIGBEE_STATE_NETWORK_JOINED    // Network available
} ZigBeeState_t;


typedef enum
{
    APP_STATE_STOP,		// no application is executed
    APP_STATE_IDLE,		// app waits for its execution time
    APP_STATE_RUN,		// application shall run
    APP_STATE_PREPSLEEP,	// application boot state
    APP_STATE_SLEEPING,		// application is sleeping
    APP_STATE_WAKEUP,		// application got woken up
    APP_STATE_SENDING,		// sending a frame
    APP_STATE_RECEIVEDFRAME	// received a sdds msg
} ApplicationState_t;


// declaration of the transport frames
BEGIN_PACK

typedef struct
{
  uint8_t        header[APS_ASDU_OFFSET];
  uint8_t 	 payload[SDDS_NET_MAX_BUF_SIZE];
  uint8_t        footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET];
} PACK AppMessageRequest_t;
END_PACK

#define PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_ENDPOINT_ID	0x0a
#define PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_PROFILE_ID	0x0F04
#define PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_CLUSTER_ID	1
#define PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_DEVICE_ID	1
#define PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_DEVICE_VERSION	1

#define SDDS_ZIGBEE_COORDINATOR_EXT_ADDR CS_APS_TRUST_CENTER_ADDRESS

#define LINK_KEY {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}


#define ZIGBEE_BROADCAST_ADDR 0xffff


#endif /* BITCLOUD_H_ */
