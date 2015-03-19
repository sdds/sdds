/*
 * BitCloud.c
 *
 *  Created on: Aug 9, 2010
 *      Author: kai
 */

// TODO retransmission
// TODO leave network
// TODO loose parent
// TODO loose child
// TODO set up group addresses
// TODO impl group addr iface from sdds
// TODO refactor netbuffref class => create with alloc of buf, init without

#include <halFCPU.h>
#include <util/delay.h>

#include "BitCloud.h"
#include "WSNVisualizer.h"

#include "sDDS/Locator.h"
#include "sDDS/NetBuffRef.h"
// BitCloud specific variables

// application interface
#include "Application.h"




/*
 *
 * Declaration of local methodes
 */

void BitCloud_AppTimer_fired(void);


// state methodes

void BitCloud_State_set(ZigBeeState_t);

void BitCloud_State_initSystem(void);
void BitCloud_State_waitToStart(void);
void BitCloud_State_joinNetwork(void);
void BitCloud_State_executeTask(void);

// send methodes impl in sdds specific impl file
rc_t BitCloud_Network_State_INIT_send(NetBuffRef buff);
rc_t BitCloud_Network_State_NETJOINING_send(NetBuffRef buff);
rc_t BitCloud_Network_State_NET_send(NetBuffRef buff);

// process frame meth, impl in sdds specific impl file
extern rc_t BitCloud_sdds_processFrame(APS_DataInd_t *indDat);

extern rc_t BitCloud_sdds_setDataRequestAddr(APS_DataReq_t* dr, Locator loc);

// zigbee specific

static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);
static void APS_DataConf(APS_DataConf_t *confInfo);

#ifdef _COORDINATOR_
void APS_DataIndCoord(APS_DataInd_t *indData);
#endif

#if defined(_ROUTER_) || defined(_ENDDEVICE_)
void APS_DataIncoming(APS_DataInd_t *indData);
#endif

struct Network_t {

    //application state not used so far?
    ZigBeeState_t zState;

    ApplicationState_t appState;

#ifndef _ENDDEVICE_
    HAL_AppTimer_t appTaskTimer;
    HAL_AppTimer_t sDDSTaskTimer;
#endif

    void (*BitCloud_State_handle)(void);
    rc_t (*send)(NetBuffRef);

    APS_DataReq_t dataRequest;
    bool dataRequestInProcess;

    SimpleDescriptor_t localDesc;
    APS_RegisterEndpointReq_t endpointParams;
    ZDO_StartNetworkReq_t networkParams;

    DeviceType_t appDeviceType;

    // network infos
    ShortAddr_t 	nodeAddr;
    ShortAddr_t		parentAddr;
    PanId_t		panID;
    uint8_t		workingChannel;
};



static struct Network_t net = {
	.zState = ZIGBEE_STATE_INITIAL,
	.appState = APP_STATE_STOP,
	.BitCloud_State_handle =  BitCloud_State_initSystem
};



void APL_TaskHandler(void)
{
    // run methode depending of state

    net.BitCloud_State_handle();

}

void BitCloud_State_set(ZigBeeState_t state)
{
    switch (state){
	case (ZIGBEE_STATE_INITIAL):
		net.zState = ZIGBEE_STATE_INITIAL;
		net.BitCloud_State_handle = BitCloud_State_initSystem;
		net.send = BitCloud_Network_State_INIT_send;
		break;
	case (ZIGBEE_STATE_NETWORK_JOINING):
		net.zState = ZIGBEE_STATE_NETWORK_JOINING;
		net.BitCloud_State_handle = BitCloud_State_joinNetwork;
		net.send = BitCloud_Network_State_NETJOINING_send;
		break;
	case (ZIGBEE_STATE_NETWORK_JOINED):
	   net.zState = ZIGBEE_STATE_NETWORK_JOINED;
	   net.BitCloud_State_handle = BitCloud_State_executeTask;
	   net.send = BitCloud_Network_State_NET_send;
	   break;
	default:
	    net.zState = ZIGBEE_STATE_INITIAL;
	    net.BitCloud_State_handle = BitCloud_State_initSystem;
	    net.send = BitCloud_Network_State_INIT_send;
    }

}

void BitCloud_State_initSystem(void)
{
    // do something with the display or the leds
    visualizeAppStarting();

#ifdef _COORDINATOR_
    // IFDEF USB2SERIAL thingy
    // start usart service
    // TODO
#endif

    net.localDesc.endpoint = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_ENDPOINT_ID;
    net.localDesc.AppProfileId = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_PROFILE_ID;
    net.localDesc.AppDeviceId = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_DEVICE_ID;
    net.localDesc.AppDeviceVersion = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_DEVICE_VERSION;
    net.localDesc.AppInClustersCount = 0;
    net.localDesc.AppInClustersList = NULL;
    net.localDesc.AppOutClustersCount = 0;
    net.localDesc.AppOutClustersList = NULL;

    net.endpointParams.simpleDescriptor = &(net.localDesc);

    // TODO get the IEEE 64 address, if not already done during compile time

#ifdef _COORDINATOR_
#ifdef _SECURITY_
    uint64_t tcAddr;
    CS_ReadParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &tcAddr);
    CS_WriteParameter(CS_UID_ID, &tcAddr);
#endif
    net.endpointParams.APS_DataInd = APS_DataIndCoord;
    net.appDeviceType = DEVICE_TYPE_COORDINATOR;
#endif

#ifdef _ROUTER_
#ifdef _SECURITY_
    uint64_t tcAddr = CCPU_TO_LE64(SDDS_ZIGBEE_COORDINATOR_EXT_ADDR);
    CS_WriteParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &tcAddr);
#endif
    net.endpointParams.APS_DataInd = APS_DataIncoming;
    net.appDeviceType = DEVICE_TYPE_ROUTER;
#endif

#ifdef _ENDDEVICE_
#ifdef _SECURITY_
    uint64_t tcAddr = CCPU_TO_LE64(SDDS_ZIGBEE_COORDINATOR_EXT_ADDR);
    CS_WriteParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &tcAddr);
#endif
    net.endpointParams.APS_DataInd = APS_DataIncoming;
    net.appDeviceType = DEVICE_TYPE_ENDDEVICE;
#endif

    // set device type
    // needed by the zigbee stack
    CS_WriteParameter(CS_DEVICE_TYPE_ID, &(net.appDeviceType));

    //  we want unique aka coordinator assigned addresses
	bool b = true;
	CS_WriteParameter(CS_NWK_UNIQUE_ADDR_ID, &b);

#ifdef _ENDDEVICE_
    bool rxOnWhenIdleFlag = false;
    CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rxOnWhenIdleFlag);
#else
    bool rxOnWhenIdleFlag = true;
    CS_WriteParameter(CS_RX_ON_WHEN_IDLE_ID, &rxOnWhenIdleFlag);
#endif

#if 0
#ifdef _SECURITY_
#ifdef _HIGH_SECURITY_
    // HIGH SECURITY

    APS_ResetKeyPairSet();
#ifdef _COORDINATOR_
    uint8_t linkKey[16] = LINK_KEY;
    ExtAddr_t extAddr = CCPU_TO_LE64(DEVICE1_EXT_ADDR);
    APS_SetLinkKey(&extAddr, linkKey);
    ExtAddr_t extAddr1 = CCPU_TO_LE64(DEVICE2_EXT_ADDR);
    APS_SetLinkKey(&extAddr1, linkKey);
#else
    uint8_t linkKey[16] = LINK_KEY;
    ExtAddr_t extAddr = CCPU_TO_LE64(SDDS_ZIGBEE_COORDINATOR_EXT_ADDR);
    APS_SetLinkKey(&extAddr, linkKey);
#endif // _COORDINATOR_
#endif // _HIGH_SECURITY_
#endif // _SECURITY_
#endif


    // init default zigbee data request
    net.dataRequest.profileId 		    = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_PROFILE_ID;
    net.dataRequest.dstAddrMode             = APS_SHORT_ADDRESS;
    net.dataRequest.dstAddress.shortAddress = CPU_TO_LE16(0);
    net.dataRequest.dstEndpoint             = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_ENDPOINT_ID;
    net.dataRequest.clusterId               = CPU_TO_LE16(PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_CLUSTER_ID);
    net.dataRequest.srcEndpoint             = PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_ENDPOINT_ID;
//    net.dataRequest.asduLength              = sizeof(dirtyAppMessage.data);
//    net.dataRequest.asdu                    = (uint8_t *)(&dirtyAppMessage.data);
//    net.dataRequest.txOptions.acknowledgedTransmission = 1;
    net.dataRequest.txOptions.fragmentationPermitted = 0;
 #ifdef _HIGH_SECURITY_
    net.dataRequest.txOptions.securityEnabledTransmission = 1;
#endif
    net.dataRequest.radius                  = 0;
    net.dataRequest.APS_DataConf            = APS_DataConf;

    // no data sending atm
    net.dataRequestInProcess = false;



    // TODO set up default group channels




    // init the application task

#ifdef _ENDDEVICE_
    // init sleep functionality
#else
    // init app task timer
    HAL_StopAppTimer(&(net.appTaskTimer));
    net.appTaskTimer.interval = APP_TIMER_TASK_INTERVALL;
    net.appTaskTimer.mode     = TIMER_ONE_SHOT_MODE;
    net.appTaskTimer.callback = BitCloud_AppTimer_fired;
#endif

    // init application?
    sdds_app_init();

    // set state to joining network
    BitCloud_State_set(ZIGBEE_STATE_NETWORK_JOINING);

    // finished yield
    SYS_PostTask(APL_TASK_ID);

}
void BitCloud_State_waitToStart(void);

void BitCloud_State_joinNetwork(void)
{
#ifdef _ENDDEVICE_
#else
    // stop app timer
    HAL_StopAppTimer(&(net.appTaskTimer));
#endif

    // visualize something
    visualizeNwkStarting();

    // start the network
    // register the callback for the network config?
    net.networkParams.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
    ZDO_StartNetworkReq(&(net.networkParams));

}

void BitCloud_AppTimer_fired(void)
{
    if (net.appState == APP_STATE_IDLE){
	net.appState = APP_STATE_RUN;
    }
    SYS_PostTask(APL_TASK_ID);
}

// execute application task
void BitCloud_State_executeTask(void)
{
	switch (net.appState)
	{
		case APP_STATE_RECEIVEDFRAME:
#ifndef _ENDDEVICE_
			HAL_StopAppTimer(&net.appTaskTimer);
#endif
			//BitCloud_SDDSFRAME_process();
			net.appState = APP_STATE_IDLE;
#ifndef _ENDDEVICE_
			HAL_StartAppTimer(&net.appTaskTimer);
#endif
			break;

		case APP_STATE_STOP:
			break;

		case APP_STATE_IDLE:
			break;

		case APP_STATE_RUN:
#ifndef _ENDDEVICE_
			HAL_StopAppTimer(&net.appTaskTimer);
#endif

			// run application task
			sdds_app_run();
			net.appState = APP_STATE_IDLE;
#ifndef _ENDDEVICE_
			HAL_StartAppTimer(&net.appTaskTimer);
#endif
			break;

		case APP_STATE_SENDING:
			break;

		default:
			break;
	}

	SYS_PostTask(APL_TASK_ID);
}


#ifdef _COORDINATOR_
// send data to uart etc pp
void APS_DataIndCoord(APS_DataInd_t *indData)
{
  visualizeAirRxFinished();

//  APL_WRITE_LOG(0x62)
//  appSendMessageToUsart((AppMessage_t *)indData->asdu);
}
#endif
#ifndef _COORDINATOR_// TODO call network incoming method
void APS_DataIncoming(APS_DataInd_t *indData)
{


    // check frame
    if (indData->status != APS_SUCCESS_STATUS){
	return ;
    }

    // check sdds msg
    if (indData->clusterId != PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_CLUSTER_ID ||
	    indData->profileId != PLATFORM_BITCLOUD_SDDS_ZIGBEE_APP_PROFILE_ID){
	return ;
    }

    // delegate the further processing to the sdds specific impl
    BitCloud_sdds_processFrame(indData);

}

#endif



static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo)
{
    // check if joining a network was a sucess
    if (confirmInfo->status == ZDO_SUCCESS_STATUS) {
	// set new state, we are in a network now :)
	BitCloud_State_set(ZIGBEE_STATE_NETWORK_JOINED);

	// visualize we are in a network now ..
	visualizeNwkStarted();

	// extract some infos
	// TODO should create a locator obj?
	net.panID	= confirmInfo->PANId;
	net.nodeAddr	= confirmInfo->shortAddr;

	net.parentAddr	= confirmInfo->parentAddr;

	net.workingChannel = confirmInfo->activeChannel;
	// register endpoint
	// register the endpoint description
	APS_RegisterEndpointReq(&(net.endpointParams));

	// start app task
#ifdef _ENDDEVICE_
#else
	// start the application
	net.appState = APP_STATE_IDLE;
	HAL_StartAppTimer(&(net.appTaskTimer));
#endif
    }
    SYS_PostTask(APL_TASK_ID);

}

void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams)
{
    nwkParams = nwkParams;

}

static void APS_DataConf(APS_DataConf_t *confInfo)
{

    // check the success of the last sending
    if (confInfo->status == APS_SUCCESS_STATUS) {
	visualizeAirTxFInished();
	net.appState = APP_STATE_IDLE;
	net.dataRequestInProcess = false;
	sdds_app_sendOK();
    } else {
	// TODO ehmm? we should do something ...
	net.appState = APP_STATE_IDLE;
	net.dataRequestInProcess = false;
    }

}

rc_t Network_send(NetBuffRef buff)
{

    return net.send(buff);
}


rc_t BitCloud_Network_State_INIT_send(NetBuffRef buff)
{
    buff = buff;
    return SDDS_RT_FAIL;
}
rc_t BitCloud_Network_State_NETJOINING_send(NetBuffRef buff)
{
    buff = buff;
    return SDDS_RT_FAIL;
}
rc_t BitCloud_Network_State_NET_send(NetBuffRef buff)
{
    if (net.dataRequestInProcess == true){
	return 1;
    }



    // set the address to the data request
    BitCloud_sdds_setDataRequestAddr(&(net.dataRequest), buff->addr);


    // not ack with broadcast!
   // net.dataRequest.txOptions.acknowledgedTransmission = 1;

    // set size
    net.dataRequest.asduLength =  (uint16_t) buff->curPos;
    // set data frame
    net.dataRequest.asdu =  (uint8_t*)buff->buff_start;
    // set flag
    net.dataRequestInProcess = true;
    // set state
    net.appState = APP_STATE_SENDING;

    visualizeAirTxStarted();

    // request data send
    APS_DataReq(&(net.dataRequest));


    return SDDS_RT_OK;
}


#ifdef _BINDING_
/***********************************************************************************
  Stub for ZDO Binding Indication

  Parameters:
    bindInd - indication

  Return:
    none

 ***********************************************************************************/
void ZDO_BindIndication(ZDO_BindInd_t *bindInd)
{
  (void)bindInd;
}

/***********************************************************************************
  Stub for ZDO Unbinding Indication

  Parameters:
    unbindInd - indication

  Return:
    none

 ***********************************************************************************/
void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
  (void)unbindInd;
}
#endif //_BINDING_
