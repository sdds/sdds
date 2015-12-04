/*
 * =====================================================================================
 *
 *       Filename:  Msg.c
 *
 *    Description:  impl file for the abstract Msg class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 09:11:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "sDDS.h"

rc_t
Msg_init(Msg_t* self, Data dataBuffer) {

//    self->next = NULL;
    if (dataBuffer != NULL) {
        self->data = dataBuffer;
    }
    /*
       self->isEmpty = true;
       self->dir = SDDS_MSG_DIR_UNSPEZ;
       self->isRead = false;
       self->isLoaned = false;
       self->isNew = false;
     */
    return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
MsgPool_getFreeMsg(struct MsgPool* self, Msg_t** msg) {

    uint8_t end = (self->start + self->count) % SDDS_TOPIC_APP_MSG_COUNT;

    *msg = &(self->pool[end]);

    if (self->count < SDDS_TOPIC_APP_MSG_COUNT) {
        self->count++;

    }
    else {
        // ring buffer full, discard oldest element
        self->start = (self->start + 1) % SDDS_TOPIC_APP_MSG_COUNT;
    }

    return SDDS_RT_OK;
}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
MsgPool_getNextMsg(struct MsgPool* self, Msg_t** msg) {

    if (self->count == 0) {
        return SDDS_RT_NODATA;
    }

    *msg = &(self->pool[self->start]);

    self->start = (self->start + 1) % SDDS_TOPIC_APP_MSG_COUNT;

    self->count--;

    return SDDS_RT_OK;
}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
MsgPool_getUnreadMsgCount(struct MsgPool* self, uint8_t* count) {

    *count = self->count;

    return SDDS_RT_OK;
}
#endif

rc_t
Msg_getData(Msg_t* self, Data* data) {
    *data = self->data;
    return SDDS_RT_OK;
}
