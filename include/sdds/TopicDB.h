/*
 * =====================================================================================
 *
 *       Filename:  TopicDB.h
 *
 *    Description:  singelton class to store all the topics within the dds 
 *    system on one device
 *
 *        Version:  1.0
 *        Created:  05.03.2010 14:59:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  TOPICDB_H_INC
#define  TOPICDB_H_INC
#include "sdds_types.h"
#include "Topic.h"
#include "NetBuffRef.h"



bool_t TopicDB_checkDomain(domainid_t domain);
Topic TopicDB_getTopic(topicid_t topic);
bool_t TopicDB_checkTopic(topicid_t topic);
Topic TopicDB_createTopic(void);

rc_t TopicDB_publishBuiltinTopic(NetBuffRef buf);

#endif   /* ----- #ifndef TOPICDB_H_INC  ----- */
