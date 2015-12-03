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
#include "os-ssal/TimeMng.h"
#include "Topic.h"

struct Locator_t;

struct _NetBuffRef_t {
    uint8_t subMsgCount;

    NetFrameBuff frame_start;
    byte_t* buff_start;
    uint8_t curPos;

    // QOS part
#if SDDS_QOS_DW_LARBUD < 65536
    time16_t sendDeadline;
#else
    time32_t sendDeadline;
#endif
    // state of the frame
    Locator_t* addr;
    Topic_t* curTopic;
    domainid_t curDomain;
};
typedef struct _NetBuffRef_t NetBuffRef_t;

rc_t
NetBuffRef_init(NetBuffRef_t* _this);
rc_t
NetBuffRef_renew(NetBuffRef_t* _this);

#ifdef UTILS_DEBUG
void
NetBuffRef_print(NetBuffRef_t* _this);
#endif
#endif   /* ----- #ifndef NETBUFFREF_H_INC  ----- */