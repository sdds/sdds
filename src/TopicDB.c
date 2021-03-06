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
#include "sDDS.h"
#include "string.h"

struct _TopicDB_t {
    Topic_t db[SDDS_TOPIC_MAX_COUNT];
    unsigned int topicCount : 4;
};
typedef struct _TopicDB_t TopicDB_t;

static TopicDB_t topicdb = {.topicCount = 0};
static unsigned int s_topicIterator = 0;

rc_t
BuiltinTopicTopic_encode(NetBuffRef_t* buff, Data data, size_t* size);

void
TopicDB_iteratorReset() {
    s_topicIterator = 0;
}
Topic_t*
TopicDB_iteratorNext() {
    return &topicdb.db[s_topicIterator++];
}

bool_t
TopicDB_iteratorHasNext() {
    return (s_topicIterator < topicdb.topicCount);
}

Topic_t*
TopicDB_createTopic(void) {
    if (topicdb.topicCount < SDDS_TOPIC_MAX_COUNT) {
        Topic_t* n = &(topicdb.db[topicdb.topicCount++]);
        memset(n, 0, sizeof(Topic_t));

        return n;
    }
    else {
        return NULL;
    }
}

bool_t
TopicDB_checkDomain(domainid_t domain) {
    for (int i = 0; i < topicdb.topicCount; i++) {
        if (topicdb.db[i].domain == domain) {
            return true;
        }
    }
    return false;
}
Topic_t*
TopicDB_getTopic(topicid_t topic) {
    for (int i = 0; i < topicdb.topicCount; i++) {
        if (topicdb.db[i].id == topic) {
            return &(topicdb.db[i]);
        }
    }
    return NULL;
}

bool_t
TopicDB_checkTopic(topicid_t topic) {
    for (int i = 0; i < topicdb.topicCount; i++) {
        if (topicdb.db[i].id == topic) {
            return true;
        }
    }
    return false;
}

// Impl for the BuiltinTopic
rc_t
BuiltinTopic_writeTopics2Buf(NetBuffRef_t* buf) {
    SNPS_writeTopic(buf, DDS_DCPS_TOPIC_TOPIC);

    for (uint8_t i = 0; i < topicdb.topicCount; i++) {
        SNPS_writeData(buf, BuiltinTopicTopic_encode, (Data) &(topicdb.db[i]));
    }

    return SDDS_RT_OK;
}

rc_t
BuiltinTopicTopic_encode(NetBuffRef_t* buff, Data data, size_t* size) {
    Topic_t* t = (Topic_t*) data;

    byte_t* start = buff->buff_start + buff->curPos;

    *size = 0;
    Marshalling_enc_uint8(start+(*size), &(t->domain));
    *size += sizeof(domainid_t);
    Marshalling_enc_uint8(start+(*size), (uint8_t*)&(t->id));
    *size += sizeof(topicid_t);

    return SDDS_RT_OK;
}
