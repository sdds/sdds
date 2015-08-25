/*
 * =====================================================================================
 *
 *       Filename:  DataSink.h
 *
 *    Description:  data sink class to receive data. Impl all datareader of the 
 *    system
 *
 *        Version:  1.0
 *        Created:  02.03.2010 19:01:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  DATASINK_H_INC
#define  DATASINK_H_INC

#include "sdds_types.h"
#include "NetBuffRef.h"

struct DataReader;
typedef struct DataReader DataReader_t;
typedef struct DataReader* DataReader;


struct DataSink_t;
typedef struct DataSink_t* DataSink;

extern DataSink dataSink;

typedef void (*On_Data_Avail_Listener)(DataReader);

rc_t DataSink_init(void);
DataReader DataSink_create_datareader(Topic topic, Qos qos, Listener listener, StatusMask sm);
rc_t DataSink_take_next_sample(DataReader _this, Data* data, DataInfo info);
rc_t DataSink_processFrame(NetBuffRef buff);
rc_t DataSink_set_on_data_avail_listener (
		DataReader _this,
		On_Data_Avail_Listener listener,
		const StatusMask sm);

rc_t DataSink_printAddr();

#endif   /* ----- #ifndef DATASINK_H_INC  ----- */
