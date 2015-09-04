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
#include "Topic.h"
#include "sDDS.h"
#include "SourceQos.h"
#include "NetBuffRef.h"
#include "BuiltinTopic.h"

#define SDDS_DATASOURCE_OK 0


struct DataWriter;
typedef struct DataWriter DataWriter_t;
typedef struct DataWriter* DataWriter;


struct DataSource;
typedef struct DataSource DataSource_t;
typedef struct DataSource* DataSource;

extern DataSource dataSource;

rc_t DataSource_init(void);

DataWriter DataSource_create_datawriter(Topic topic, Qos qos, Listener list, StatusMask mask);
rc_t DataSource_writeAddress(DataWriter _this, char *addr);

rc_t DaraSource_getDataWrites(DDS_DCPSPublication *pt, int *len);


#ifdef SDDS_TOPIC_HAS_SUB
rc_t DataSource_write(DataWriter _this, Data data, void* waste);
#endif// SDDS_TOPIC_HAS_SUB



#endif   /* ----- #ifndef DATASOURCE_H_INC  ----- */
