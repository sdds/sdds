/*
 * =====================================================================================
 *
 *       Filename:  NetBuffRef.h
 *
 *    Description:  class to wrap the networkbuffer and store the state of the 
 *    protocol etc
 *
 *        Version:  1.0
 *        Created:  24.02.2010 13:56:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  NETBUFFREF_H_INC
#define  NETBUFFREF_H_INC
#include "sdds_types.h"
#include "sDDS.h"
#include "NetFrameBuff.h"
#include "TimeMng.h"
#include "Topic.h"

struct Locator_t;

struct NetBuffRef_t{
//    uint8_t byteCount;
    uint8_t subMsgCount;

    NetFrameBuff frame_start;
    byte_t* buff_start;
    uint8_t curPos;

    // QOS part
    pointInTime_t  sendDeadline;

    // state of the frame
    struct Locator_t* addr;
    Topic curTopic;
    domainid_t curDomain;
};
typedef struct NetBuffRef_t* NetBuffRef;

rc_t NetBuffRef_init(NetBuffRef _this);
rc_t NetBuffRef_renew(NetBuffRef _this);

#ifdef _DEBUG
void NetBuffRef_print(NetBuffRef _this);
#endif
#endif   /* ----- #ifndef NETBUFFREF_H_INC  ----- */
