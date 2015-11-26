/*
 * =====================================================================================
 *
 *       Filename:  Topic.c
 *
 *    Description:  impl file for the abstract part of the topic class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 09:27:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "Topic.h"
#include "sdds_types.h"
#include "Msg.h"

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t Topic_getFreeMsg(Topic_t *_this, Msg_t **msg) {
	return MsgPool_getFreeMsg(&(_this->msg), msg);
}
rc_t Topic_getNextMsg(Topic_t *_this, Msg_t **msg) {
	return MsgPool_getNextMsg(&(_this->msg), msg);
}
rc_t Topic_getUnreadMsgCount(Topic_t *_this, uint8_t* count) {
	return MsgPool_getUnreadMsgCount(&(_this->msg), count);
}
#endif

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t Topic_addRemoteDataSink(Topic_t *_this, Locator_t* addr)
{
	if (_this == NULL || addr == NULL){
		return SDDS_RT_BAD_PARAMETER;
	}
	if (_this->dsinks.list == NULL) {
		_this->dsinks.list = addr;
		Locator_upRef(addr);
		return SDDS_RT_OK;
	}

	Locator_t* tmp = _this->dsinks.list;

	if (Locator_contains(tmp, addr) != SDDS_RT_OK) {
		return Locator_addToList(tmp, addr);
	}

	return SDDS_RT_OK;
}
#endif

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t Topic_addRemoteDataSource(Topic_t *_this, Locator_t* addr)
{
	if (_this == NULL || addr == NULL){
		return SDDS_RT_BAD_PARAMETER;
	}

	// first element in list
	if (_this->dsources.list == NULL) {
		_this->dsources.list = addr;
		Locator_upRef(addr);
		return SDDS_RT_OK;
	}
	Locator_t* tmp = _this->dsources.list;

	if (Locator_contains(tmp, addr) != SDDS_RT_OK) {
		return Locator_addToList(tmp, addr);
	}

	return SDDS_RT_OK;
}
#endif


