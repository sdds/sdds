/*
 * =====================================================================================
 *
 *       Filename:  TopicDB.c
 *
 *    Description:  impl file for the topic database
 *
 *        Version:  1.0
 *        Created:  08.03.2010 14:41:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "TopicDB.h"
#include "sdds_types.h"
#include "BuiltinTopic.h"
#include "Marshalling.h"
#include "SNPS.h"

struct TopicDB_t {
    struct Topic_t db[SDDS_TOPIC_MAX_COUNT];
   // struct Topic_t db[4];
    unsigned int topicCount : 4;
};

static struct TopicDB_t topicdb = {.topicCount = 0};

rc_t BuiltinTopicTopic_encode(byte_t* buff, Data data, size_t* size);

Topic TopicDB_createTopic(void)
{
	if (topicdb.topicCount < SDDS_TOPIC_MAX_COUNT){
		Topic n = &(topicdb.db[topicdb.topicCount++]);
		memset(n, 0, sizeof(struct Topic_t));
		n->msg.count = 0;
		n->msg.start = 0;

		return n;
	} else {
		return NULL;
	}
}

bool_t TopicDB_checkDomain(domainid_t domain)
{
    for (int i = 0; i < topicdb.topicCount; i++){
	if (topicdb.db[i].domain == domain){
	    return true;
	}
    }
    return false;
}
Topic TopicDB_getTopic(topicid_t topic)
{
    for (int i = 0; i < topicdb.topicCount; i++){
	if (topicdb.db[i].id == topic){
	    return &(topicdb.db[i]);
	}
    }
    return NULL;
}

bool_t TopicDB_checkTopic(topicid_t topic)
{
    for (int i = 0; i < topicdb.topicCount; i++){
	if (topicdb.db[i].id == topic){
	    return true;
	}
    }
    return false;
}

// Impl for the BuiltinTopic
rc_t BuiltinTopic_writeTopics2Buf(NetBuffRef buf)
{
    SNPS_writeTopic(buf, DDS_DCPS_TOPIC_TOPIC);

    for (uint8_t i = 0; i < topicdb.topicCount; i++){
	SNPS_writeData(buf, BuiltinTopicTopic_encode, (Data) &(topicdb.db[i]));
    }

    return SDDS_RT_OK;
}

rc_t BuiltinTopicTopic_encode(byte_t* buff, Data data, size_t* size)
{
    Topic t = (Topic) data;
    *size = 0;
    Marshalling_enc_uint8(buff+(*size), &(t->domain));
    *size += sizeof(domainid_t);
    Marshalling_enc_uint8(buff+(*size), &(t->id));
    *size += sizeof(topicid_t);

    return SDDS_RT_OK;
}
