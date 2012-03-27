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


#ifdef sDDS_TOPIC_HAS_PUB
rc_t Topic_getFreeMsg(Topic _this, Msg* msg)
{
    for (int i = 0; i < sDDS_TOPIC_APP_MSG_COUNT; i++){
	if (_this->msg.pool[i].isEmpty == true){
	    // found one
	    _this->msg.pool[i].isEmpty = false;
	    *msg = &(_this->msg.pool[i]);
	    return SDDS_RT_OK;
	}
    }
    // nothing found yet?
    // use a allready read data
    for (int i = 0; i < sDDS_TOPIC_APP_MSG_COUNT; i++){
	if (_this->msg.pool[i].isRead == true && _this->msg.pool[i].isLoaned == false){
	    // empty it
	    Msg_init(&(_this->msg.pool[i]), NULL);
	    _this->msg.pool[i].isEmpty = false;
	    *msg = &(_this->msg.pool[i]);
	    return SDDS_RT_OK;
	}
    }

    // no free msg!
    return SDDS_RT_FAIL;
}
#endif


#ifdef sDDS_TOPIC_HAS_SUB
rc_t Topic_addDataSink(Topic _this, Locator addr)
{

    Locator tmp = _this->dsinks.list;

    while (tmp->next != NULL) tmp = tmp->next;
    tmp->next = addr;
    // FIXME here is a big logical problem
    // what if the locator is allreay in another list?
    addr->next = NULL;

    return SDDS_RT_OK;

}
#endif
#ifdef sDDS_TOPIC_HAS_PUB
rc_t Topic_addDataSource(Topic _this, Locator addr)
{
    if (_this == NULL || addr == NULL){
	return SDDS_RT_FAIL;
    }
    Locator tmp = _this->dsources.list;
    while (tmp->next != NULL) tmp = tmp->next;
    tmp->next = NULL;
    return SDDS_RT_OK;
}
#endif


