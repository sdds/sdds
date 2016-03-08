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

#include "List.h"
#include "gen_constants.h"

struct Locator_t;

struct _NetBuffRef_t {
    uint8_t subMsgCount;

    NetFrameBuff frame_start;
    byte_t* buff_start;
    uint16_t curPos;

    // QOS part
#if SDDS_QOS_DW_LATBUD < 65536
    time16_t sendDeadline;
    time16_t latBudDuration;
#else
    time32_t sendDeadline;
    time32_t latBudDuration;
#endif
    bool_t bufferOverflow : 1;


    // state of the frame
    List_t* locators;
    Topic_t* curTopic;
    domainid_t curDomain;
};

rc_t
NetBuffRef_init(NetBuffRef_t* _this);
rc_t
NetBuffRef_renew(NetBuffRef_t* _this);

#ifdef UTILS_DEBUG
void
NetBuffRef_print(NetBuffRef_t* _this);
#endif
#endif   /* ----- #ifndef NETBUFFREF_H_INC  ----- */
