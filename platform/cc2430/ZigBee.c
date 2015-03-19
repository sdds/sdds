#include "Locator.h"
#include "LocatorDB.h"
#include "Network.h"
#include "Log.h"
#include "Memory.h"
#include "NetBuffRef.h"

#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"

#include "aps_groups.h"


#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#include "GenericApp.h"

// This list should be filled with Application specific Cluster IDs.
#if 1
const cId_t GenericApp_ClusterList[PLATFORM_CC2430_GENERICAPP_MAX_CLUSTERS] =
{
  PLATFORM_CC2430_GENERICAPP_CLUSTERID
};

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

#endif


struct Network_t {

    // task id of the network module
    byte taskID;
  
    devStates_t nwkState;
  
  // This is the Endpoint/Interface description.  It is defined here, but
// filled-in in GenericApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
    endPointDesc_t epDesc;
    
    byte transID;
    aps_Group_t ddsGroup;
};

struct ZigBeeLocator_t{
    struct Locator_t loc;
    afAddrType_t addr;
};

static struct Network_t net;
static struct NetBuffRef_t inBuff;


//void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
//void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void GenericApp_SendTheMessage( void );

void IncomingFrame (afIncomingMSGPacket_t* msg);

UINT16 ZigBee_ProcessEvent( byte task_id, UINT16 events )
{
    afIncomingMSGPacket_t *MSGpkt;
    afDataConfirm_t *afDataConfirm;

    // Data Confirmation message fields
    byte sentEP;
    ZStatus_t sentStatus;
    byte sentTransID;       // This should match the value sent
    
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( net.taskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          //GenericApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          IncomingFrame (MSGpkt);
          //GenericApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          net.nwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (net.nwkState == DEV_ZB_COORD)
              || (net.nwkState == DEV_ROUTER)
              || (net.nwkState == DEV_END_DEVICE) )
          {
            // Start sending "the" message in a regular interval.
          //  osal_start_timerEx( net.taskID,
          //                      PLATFORM_CC2430_GENERICAPP_SEND_MSG_EVT,
          //                      PLATFORM_CC2430_GENERICAPP_SEND_MSG_TIMEOUT );
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( net.taskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  
   return 0;
}

rc_t Network_init(void){
    return SDDS_RT_OK;
}

void ZigBee_init( byte task_id )
{
    osal_memset(&net, 0, sizeof(struct Network_t));
    net.taskID = task_id;
    net.nwkState = DEV_INIT;
    net.transID = 0;
    
    // Fill out the endpoint description.
  net.epDesc.endPoint = PLATFORM_CC2430_GENERICAPP_ENDPOINT;
  net.epDesc.task_id = &(net.taskID);
  
  net.epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  net.epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &(net.epDesc) );
  
  
  // set up the dds group 
  net.ddsGroup.ID = 0x0001;
  osal_memcpy( net.ddsGroup.name, "DDS", 3);
  aps_AddGroup( PLATFORM_CC2430_GENERICAPP_ENDPOINT, &(net.ddsGroup) );
  // TODO?
 //   ZDO_RegisterForZDOMsg( net.taskID, End_Device_Bind_rsp );
 //   ZDO_RegisterForZDOMsg( net.taskID, Match_Desc_rsp );
  
        // init the incoming frame buffer
 //   NetBuffRef_init(&inBuff);
    
}



/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      GenericApp_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
#if 0
void GenericApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined(BLINK_LEDS)
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            //GenericApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            //GenericApp_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            //GenericApp_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}
#endif


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      GenericApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void IncomingFrame (afIncomingMSGPacket_t* msg)
{
          NetBuffRef buff = (NetBuffRef) &inBuff;
          if (msg->clusterId == PLATFORM_CC2430_GENERICAPP_CLUSTERID) {
        
    	NetBuffRef_renew(buff);
        buff->buff_start = msg->cmd.Data;
        
        struct ZigBeeLocator_t sloc;
	osal_memcpy(&(sloc.addr), &(msg->srcAddr), sizeof(afAddrType_t));
        
        Locator loc;
	if (LocatorDB_findLocator((Locator)&sloc, &loc) != SDDS_RT_OK){
	    // not found we need a new one
	    if (LocatorDB_newLocator(&loc) != SDDS_RT_OK){
		Log_error("No free Locator objects\n");
		return;
	    }
	    osal_memcpy(&(((struct ZigBeeLocator_t*)loc)->addr), &(msg->srcAddr), sizeof(afAddrType_t));

	}
	// up ref counter
	Locator_upRef(loc);
	loc->isEmpty = false;
	loc->isSender = true;
	// add locator to the netbuffref
	inBuff.addr = loc;
	

	// invoke the datasink handler
	DataSink_processFrame(buff);
	Locator_downRef(loc);
  }
}
#if 0
void GenericApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case PLATFORM_CC2430_GENERICAPP_CLUSTERID:
      // "the" message
#if defined( LCD_SUPPORTED )
      HalLcdWriteScreen( (char*)pkt->cmd.Data, "rcvd" );
#elif defined( WIN32 )
      WPRINTSTR( pkt->cmd.Data );
#endif
      break;
  }
}
#endif
rc_t Network_send(NetBuffRef buff) {
    
    afAddrType_t* dest = (afAddrType_t*) &(((struct ZigBeeLocator_t*) buff->addr)->addr);
    
    if ( AF_DataRequest( dest, &(net.epDesc),
                       PLATFORM_CC2430_GENERICAPP_CLUSTERID,
                       (byte)buff->curPos,
                       (byte *)buff->buff_start,
                       &(net.transID),
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
          return SDDS_RT_OK;   
  }
  else
  {
    // Error occurred in request to send.
      return SDDS_RT_FAIL;
  }
    

}
rc_t Network_getFrameBuff(NetFrameBuff* buff)
{
    rc_t ret = Memory_allocNetFrame(buff);
    if (ret != SDDS_RT_OK)
	return SDDS_RT_NOMEM;

    (*buff)->size = SDDS_NET_MAX_BUF_SIZE;
    return SDDS_RT_OK;
}

rc_t Network_getPayloadBegin(size_t* startByte)
{
    // payload starts at the begining of the buffer, address is provided 
    // seperatly
    // buffer starts after the struct
    *startByte = sizeof(struct NetFrameBuff_t);

    return SDDS_RT_OK;
}
size_t Network_locSize()
{
    return sizeof(struct ZigBeeLocator_t);
}
bool_t Locator_isEqual(Locator l1, Locator l2)
{
    struct ZigBeeLocator_t* a = (struct ZigBeeLocator_t*) l1;
    struct ZigBeeLocator_t* b = (struct ZigBeeLocator_t*) l2;
    if (a->addr.addr.shortAddr == b->addr.addr.shortAddr) {
	return true;
    } else {
	return false;
    }
}

rc_t Network_createLocator(Locator* loc)
{

    if (Memory_allocLocator(loc) != SDDS_RT_OK){
	return SDDS_RT_NOMEM;
    }
    struct ZigBeeLocator_t* l = (struct ZigBeeLocator_t*) *loc;
    l->loc.next = NULL;
    
    l->addr.addrMode = (afAddrMode_t) AddrBroadcast;
//    l->addr.endPoint = 0;
    l->addr.endPoint = PLATFORM_CC2430_GENERICAPP_ENDPOINT;
    l->addr.addr.shortAddr = 0xffff;
    
    return SDDS_RT_OK;
}