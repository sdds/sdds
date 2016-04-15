/*
 * =====================================================================================
 *
 *       Filename:  DataSource.h
 *
 *    Description:  Header File for the DataSource class. Object is dedicated to
 *    handle the publishing of data to the dds network.
 *
 *        Version:  1.0
 *        Created:  22.02.2010 14:53:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  DATASOURCE_H_INC_
#define  DATASOURCE_H_INC_
#include "sDDS.h"

#define SDDS_DATASOURCE_OK 0

struct _DataSource_t;
typedef struct _DataSource_t DataSource_t;

extern DataSource_t* dataSource;

rc_t
DataSource_init(void);

DataWriter_t*
DataSource_create_datawriter(Topic_t* topic, Qos qos, Listener list, StatusMask mask);

rc_t
DataSource_writeAddress(DataWriter_t* _this, castType_t castType, addrType_t addrType, uint8_t* addr, uint8_t addrLen);

#if defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataSource_getDataWrites(DDS_DCPSPublication* pt, int* len);
#endif

#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
Reliable_DataWriter_t*
DataSource_DataWriter_by_topic(topicid_t id);
#endif

#if defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataSource_getTopic(DDS_DCPSSubscription* st, topicid_t id, Topic_t** topic);
#endif

#if defined(SDDS_TOPIC_HAS_SUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) \
 || defined(SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
rc_t
DataSource_write(DataWriter_t* _this, Data data, void* waste);
#endif// SDDS_TOPIC_HAS_SUB

#ifdef UTILS_DEBUG
void print_Pointer();
#endif

#endif   /* ----- #ifndef DATASOURCE_H_INC  ----- */
