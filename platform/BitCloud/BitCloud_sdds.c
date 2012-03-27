/*
 * BitCloud_sdds.c
 *
 *  Created on: 22 Mar 2011
 *      Author: k_beckma
 *
 *      Impl file for the sdds specific impl
 */

#include "BitCloud.h"

// SDDS includes
#include "sDDS/Network.h"
#include "sDDS/sdds_types.h"
#include "sDDS/NetBuffRef.h"
#include "sDDS/LocatorDB.h"
#include "sDDS/DataSink.h"
#include "os-ssal/Memory.h"


rc_t BitCloud_sdds_setDataRequestAddr(APS_DataReq_t* dr, Locator loc );

rc_t BitCloud_sdds_processFrame(APS_DataInd_t *indDat);

// static incoming netbuff structure
static struct NetBuffRef_t inBuff;

// definition of the ZigBee locator
struct ZigBeeLocator_t{
    struct Locator_t loc;
    APS_Address_t addr;
    APS_AddrMode_t addrMode;

};

// definition of a wrapper for the frame buffer and a app msg req form zigbee
struct NetFrameBuff_Wrapper_t {
    struct NetFrameBuff_t buf;
    AppMessageRequest_t frame;
};


// declaration of a pool for the network frame buffer
struct BitCloud_NetworkFramePool_t {
    struct NetFrameBuff_Wrapper_t pool[1 + sDDS_NET_MAX_OUT_QUEUE];
    uint8_t max : 4;
    uint8_t size : 4;
};

// definition of a static space for the frame pool
static struct BitCloud_NetworkFramePool_t netFramePool =
{
    .max = 1 + sDDS_NET_MAX_OUT_QUEUE,
    .size = 0
};

// declaration of a locator obj pool
struct BitCloud_LocatorPool_t {
    struct ZigBeeLocator_t pool[sDDS_NET_MAX_LOCATOR_COUNT+1]; //FIXME
    uint8_t max;
    uint8_t size;
};

// defintion of a static space for the locator pool

static struct BitCloud_LocatorPool_t locatorPool =
{
	.max = sDDS_NET_MAX_LOCATOR_COUNT,
	.size = 0
};

rc_t BitCloud_sdds_processFrame(APS_DataInd_t *indData)
{
    // get locator
    struct ZigBeeLocator_t recvLoc;

    recvLoc.addr 	= indData->srcAddress;
    recvLoc.addrMode 	= indData->srcAddrMode;

    Locator loc;

    // find locator or create a new one
    if (LocatorDB_findLocator((Locator) &recvLoc, &loc) != SDDS_RT_OK) {
	// not found we need a new one
	if (LocatorDB_newLocator(&loc) != SDDS_RT_OK) {
	    loc = NULL;
	  //  sdds_app_fail();
	    return -1;

	}

	((struct ZigBeeLocator_t*) loc)->addr = indData->srcAddress;
	((struct ZigBeeLocator_t*) loc)->addrMode = indData->srcAddrMode;

	switch (indData->srcAddrMode) {
	    case (APS_SHORT_ADDRESS):
		if (indData->srcAddress.shortAddress == ZIGBEE_BROADCAST_ADDR) {
		    loc->type = SDDS_LOCATOR_TYPE_BROAD;
		    break;
		}
	    case (APS_EXT_ADDRESS):
		loc->type = SDDS_LOCATOR_TYPE_UNI;
		break;
	    case (APS_GROUP_ADDRESS):
		loc->type = SDDS_LOCATOR_TYPE_MULTI;
		break;
	    default:
		loc->type = SDDS_LOCATOR_TYPE_UNI;

	}

    }
    // up ref counter
    Locator_upRef(loc);
    loc->isEmpty = false;
    loc->isSender = true;

    NetBuffRef_renew(&inBuff);
    // get data and locator to inbuf
    inBuff.addr = loc;
    inBuff.buff_start = indData->asdu;


    rc_t ret = DataSink_processFrame(&inBuff);

    if (ret != SDDS_RT_OK){

    }

    Locator_downRef(loc);

    return 0;
}




rc_t BitCloud_sdds_setDataRequestAddr(APS_DataReq_t* dr, Locator loc )
{

    struct ZigBeeLocator_t* dest = ((struct ZigBeeLocator_t*) loc);
    // get/set address
    // todo abhaenig von typ machen
    // check addr type
    /*
    switch (buff->addr->type){
	case (SDDS_LOCATOR_TYPE_MULTI):
		net.dataRequest.dstAddrMode = APS_GROUP_ADDRESS;
		break;
	case (SDDS_LOCATOR_TYPE_UNI) :
	case (SDDS_LOCATOR_TYPE_BROAD):
		net.dataRequest.dstAddrMode = APS_SHORT_ADDRESS;
		break;
    };
    */
    dr->dstAddress.shortAddress = dest->addr.shortAddress;
    dr->dstAddrMode = dest->addrMode;

    return SDDS_RT_OK;
}


rc_t Network_getFrameBuff(NetFrameBuff* buff)
{

    // get frame from pool

    if (netFramePool.size < netFramePool.max){

	struct NetFrameBuff_Wrapper_t* tmp = &(netFramePool.pool[netFramePool.size++]);

	tmp->buf.size = sDDS_NET_MAX_BUF_SIZE;

	*buff = &(tmp->buf);

	return SDDS_RT_OK;
    } else {
	return SDDS_RT_NOMEM;
    }
}
rc_t Network_getPayloadBegin(size_t* startByte)
{
    *startByte = APS_ASDU_OFFSET + sizeof(NetFrameBuff);
    return SDDS_RT_OK;
}

rc_t Network_createLocator(Locator* loc)
{
    if (locatorPool.size < locatorPool.max){

	struct ZigBeeLocator_t* l = &(locatorPool.pool[locatorPool.size++]);

	l->loc.next = NULL;
	//l->loc.type = SDDS_LOCATOR_TYPE_BROAD;
	l->loc.type = SDDS_LOCATOR_TYPE_UNI;
	//l->addr.shortAddress = 0xffff;
	//l->addr.shortAddress = 0x0023;
	l->addr.shortAddress = 0x0042;
	l->addrMode = APS_SHORT_ADDRESS;

	*loc = (Locator) l;

	return SDDS_RT_OK;
    } else {
	return SDDS_RT_NOMEM;
    }

}

rc_t Network_init(void){
    // init in buffer
    // do not use the init methode, because we do not need a buffer frame
    // should be changed in the netbuffref class TODO

    // buffer is provided by bitcload, have to be assigned after receiving
    NetBuffRef_renew(&inBuff);
    return SDDS_RT_OK;
}

bool_t Locator_isEqual(Locator l1, Locator l2)
{
    struct ZigBeeLocator_t* a = (struct ZigBeeLocator_t*) l1;
    struct ZigBeeLocator_t* b = (struct ZigBeeLocator_t*) l2;

    if (a->addrMode != b->addrMode) return false;

    switch (a->addrMode) {
	case (APS_GROUP_ADDRESS):
	    if (a->addr.groupAddress == b->addr.groupAddress) return true;
	    break;
	case (APS_SHORT_ADDRESS):
	    if (a->addr.shortAddress == b->addr.shortAddress) return true;
		break;
	case (APS_EXT_ADDRESS):
	    if (a->addr.extAddress == b->addr.extAddress) return true;
		break;
	default:
	    break;
    }
    return false;
}




size_t Network_locSize(void)
{
    return sizeof(struct ZigBeeLocator_t);
}
