/*
 * =====================================================================================
 *
 *       Filename:  sDDS.c
 *
 *    Description:  Impl file for the sDDS thingy
 *
 *        Version:  1.0
 *        Created:  22.02.2010 15:50:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "sdds/sDDS.h"
#include "os-ssal/Memory.h"
#include "sdds/Network.h"
#include "sdds/Topic.h"
#include "sdds/TopicDB.h"
#include "sdds/LocatorDB.h"
#include "sdds/Msg.h"
#include "sdds/DataSink.h"
#include "sdds/DataSource.h"
#include "dds/DDS_DCPS.h"
#include "Foo.h"
#include "sdds/Marshalling.h"
#include "sdds/TopicMarshalling.h"

#include <stdio.h>
#include <string.h>
#include "CONSTANTS.h"


DDS_FooDataReader fooDataReader;
DDS_Topic fooTopic;

// data buffer for the msg
Foo fooBufferPool[SDDS_TOPIC_APP_MSG_COUNT];

Topic FooTopic_create(void);

rc_t sDDS_init()
{
    Memory_init();
    LocatorDB_init();
    Network_init();
    // init datasink
    DataSink_init();

    fooTopic = (DDS_Topic) FooTopic_create();
    Topic topic = (Topic) fooTopic;

    // create the datareader for the topic
    fooDataReader = (DDS_FooDataReader) DataSink_create_datareader(topic, NULL, NULL, 0);

    return SDDS_RT_OK;
}


// impl of the datareader read next sample methode
//

rc_t TopicMarshalling_decode(byte_t* buff, Data data, size_t* size)
{
    Foo* d = (Foo*) data;
    size_t decoded = 0;
    // should i check here if the size matches?
    if (*size != sizeof(Foo)){
	printf("Error? size mismatch\n");
    }
    Marshalling_dec_int32(buff, &(d->temperatur));
    decoded += sizeof(d->temperatur);
    Marshalling_dec_uint16((buff+decoded), &(d->id));
    decoded += sizeof(d->id);
    *size = decoded;

    return SDDS_RT_OK;
}
rc_t Data_cpy(Data dest, Data source)
{
    memcpy(dest, source, sizeof(Foo));
    return SDDS_RT_OK;
}

Topic FooTopic_create(void)
{

    Topic topic;
    topic = TopicDB_createTopic();

    topic->Data_decode = TopicMarshalling_decode;
    topic->domain = 15;
    topic->id = 1;
    topic->Data_cpy = Data_cpy;

    // init the msg pool
    for (int i = 0; i < SDDS_TOPIC_APP_MSG_COUNT; i++){
	Msg_init(&(topic->msg.pool[i]), (Data) &(fooBufferPool[i]));
    }


    return topic;
}

DDS_ReturnCode_t DDS_FooDataReader_take_next_sample (
    DDS_DataReader _this,
    Foo** data_values,
    DDS_SampleInfo *sample_info)
{
    // find new sample
    // invoce the methode of the datasink
    rc_t ret = DataSink_take_next_sample((DataReader) _this, (Data*) data_values, (DataInfo) sample_info);

    if (ret == SDDS_RT_NODATA)
	return DDS_RETCODE_NO_DATA;
    if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

    return DDS_RETCODE_ERROR;
}

