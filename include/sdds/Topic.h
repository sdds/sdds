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

struct datasources{
    Locator list;
    uint8_t count;
};
struct datasinks{
    Locator list;
    uint8_t count;
};
struct Topic_t {

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
    struct datasinks dsinks;
    rc_t (*Data_encode)(byte_t* buff, Data data, size_t* size);
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
    struct datasources dsources;
#endif

    struct MsgPool msg;

    rc_t (*Data_decode)(byte_t* buff, Data data, size_t* size);
    rc_t (*Data_cpy)(Data dest, Data source);

    domainid_t domain;
    topicid_t id;
};				/* ----------  end of struct Topic  ---------- */

typedef struct Topic_t* Topic;


#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t Topic_getFreeMsg(Topic _this, Msg_t **msg);
rc_t Topic_getNextMsg(Topic _this, Msg_t **msg);
rc_t Topic_getUnreadMsgCount(Topic _this, uint8_t* count);
#endif

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
/**
 * Add the address/locator object of a remote node as a Data sink,
 * aka receiver of data samples of this, to the topic.
 *
 * @param _this Pointer to object
 * @param addr Locator object of the remote node
 * @return SDDS return code @see rc_t
 */
rc_t Topic_addRemoteDataSink(Topic _this, Locator addr);
#endif

#if defined (SDDS_TOPIC_HAS_PUB) && (SDDS_TOPIC_DYNAMIC)
/**
 * Remove the address/locator object of a data sink remote node
 * from this topic. The remote datareader wont get any data samples
 * afterwards.
 *
 * @param _this Pointer to the class object
 * @param addr Locator object to remove
 * @return sdds return code @see rc_t
 */
rc_t Topic_removeRemoteDataSink(Topic _this, Locator addr);
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
/**
 * Add a address/locator object of a remote data source node
 * aka. sender/publisher of data samples for this topic, to the
 * topic.
 * @param _this Pointer to the object
 * @param addr Locator object of the remote node
 * @return sdds return code @see rc_t
 */
rc_t Topic_addRemoteDataSource(Topic _this, Locator addr);
#endif

#if defined (SDDS_TOPIC_HAS_SUB) && (SDDS_TOPIC_DYNAMIC)
/**
 * Remove the address/locator object of a remote data source node
 * from this topic.
 *
 * @param _this Pointer to the class object
 * @param addr Locator object to remove
 * @return sdds return code @see rc_t
 */
rc_t Topic_removeRemoteDataSink(Topic _this, Locator addr);
#endif

//rc_t Topic_addRemoteDataSink(Topic _this, Locator addr);

#endif   /* ----- #ifndef TOPIC_H_INC  ----- */
