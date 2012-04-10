/*
 * =====================================================================================
 *
 *       Filename:  Topic.h
 *
 *    Description:  Header for the sDDS Topic class.
 *
 *        Version:  1.0
 *        Created:  22.02.2010 14:58:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  TOPIC_H_INC
#define  TOPIC_H_INC

#include "sDDS.h"
#include "sdds_types.h"
#include "Locator.h"
#include "Msg.h"


struct MsgPool{
    struct Msg_t pool[sDDS_TOPIC_APP_MSG_COUNT];
};

struct datasources{
    Locator list;
    uint8_t count;
};
struct datasinks{
    Locator list;
    uint8_t count;
};
struct Topic_t {

#ifdef sDDS_TOPIC_HAS_SUB
    struct datasinks dsinks;
    rc_t (*Data_encode)(byte_t* buff, Data data, size_t* size);
#endif

#ifdef sDDS_TOPIC_HAS_PUB
    struct datasources dsources;
#endif

    struct MsgPool msg;
    rc_t (*Data_decode)(byte_t* buff, Data data, size_t* size);
    rc_t (*Data_cpy)(Data dest, Data source);

    domainid_t domain;
    topicid_t id;
};				/* ----------  end of struct Topic  ---------- */

typedef struct Topic_t* Topic;


rc_t Topic_getFreeMsg(Topic _this, Msg* msg);


#ifdef sDDS_TOPIC_HAS_SUB
rc_t Topic_addDataSink(Topic _this, Locator addr);
#endif
#ifdef sDDS_TOPIC_HAS_PUB
rc_t Topic_addDataSource(Topic _this, Locator addr);
#endif

#endif   /* ----- #ifndef TOPIC_H_INC  ----- */
