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
#include "Msg.h"
#include "sdds_types.h"
#include "BuiltinTopic.h"

rc_t Msg_init(Msg_t *_this, Data dataBuffer)
{

//    _this->next = NULL;
    if (dataBuffer != NULL) {
    	_this->data = dataBuffer;
    }
    /*
    _this->isEmpty = true;
    _this->dir = SDDS_MSG_DIR_UNSPEZ;
    _this->isRead = false;
    _this->isLoaned = false;
    _this->isNew = false;
  */
    return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t MsgPool_getFreeMsg(struct MsgPool* _this, Msg_t **msg) {

	uint8_t end = (_this->start + _this->count) % SDDS_TOPIC_APP_MSG_COUNT;

	*msg = &(_this->pool[end]);

	if (_this->count < SDDS_TOPIC_APP_MSG_COUNT) {
		_this->count++;

	} else {
		// ring buffer full, discard oldest element
		_this->start = (_this->start + 1) % SDDS_TOPIC_APP_MSG_COUNT;
	}

	return SDDS_RT_OK;
}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t MsgPool_getNextMsg(struct MsgPool* _this, Msg_t **msg) {

	if (_this->count == 0) {
		return SDDS_RT_NODATA;
	}

	*msg = &(_this->pool[_this->start]);

	_this->start = (_this->start + 1) % SDDS_TOPIC_APP_MSG_COUNT;

	_this->count--;

	return SDDS_RT_OK;
}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t MsgPool_getUnreadMsgCount(struct MsgPool* _this, uint8_t* count) {

	*count = _this->count;

	return SDDS_RT_OK;
}
#endif

rc_t Msg_getData(Msg_t *_this, Data* data) {

	*data = _this->data;

	return SDDS_RT_OK;
}
