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
#include "sdds/LocatorDB.h"
#include "sdds/Network.h"
#include "sdds/Topic.h"
#include "sdds/TopicDB.h"
#include "sdds/LocatorDB.h"
#include "sdds/Msg.h"
#include "sdds/DataSource.h"
#include "sdds/DataSink.h"
#include "dds/DDS_DCPS.h"
#include "Foo.h"
#include "sdds/Marshalling.h"
#include "sdds/TopicMarshalling.h"

#include <stdio.h>
#include <string.h>


DDS_FooDataReader fooDataReader;
DDS_FooDataWriter fooDataWriter;
DDS_Topic fooTopic;

// data buffer for the msg
Foo fooBufferPool[SDDS_TOPIC_APP_MSG_COUNT];
#define SIZE_LONG 4
#define SIZE_USHORT 2

Topic FooTopic_create(void);

rc_t sDDS_init()
{
    Memory_init();
    LocatorDB_init();
    Network_init();
    DataSource_init();
    //ds
    // init datasink
    DataSink_init();

    fooTopic = (DDS_Topic) FooTopic_create();
    Topic topic = (Topic) fooTopic;

    fooDataWriter = (DDS_FooDataWriter)  DataSource_create_datawriter(topic, NULL, NULL, NULL);
    // create the datareader for the topic
    fooDataReader = (DDS_FooDataReader) DataSink_create_datareader(topic, NULL, NULL, 0);

    return SDDS_RT_OK;
}


DDS_ReturnCode_t DDS_FooDataWriter_write(
    DDS_DataWriter _this,
    const Foo* instance_data,
    const DDS_InstanceHandle_t handle)
{

    rc_t rc; 

    rc = DataSource_write((DataWriter) _this, (Data) instance_data, (void*) handle);
    if (rc == SDDS_RT_OK) {
	return DDS_RETCODE_OK;
    } else {
	return DDS_RETCODE_ERROR;
    }

}

rc_t TopicMarshalling_encode(byte_t* buff, Data data, size_t* size)
{
    Foo* d = (Foo*) data;
    *size = 0;
    Marshalling_enc_int32(buff, &(d->temperatur));
    *size += SIZE_LONG;
    Marshalling_enc_uint16((buff+(*size)), &(d->id));
    *size += SIZE_USHORT;

    return SDDS_RT_OK;
}
// impl of the datareader read next sample methode
//

rc_t TopicMarshalling_decode(byte_t* buff, Data data, size_t* size)
{
    Foo* d = (Foo*) data;
    size_t decoded = 0;
    // should i check here if the size matches?
    // Foo is 6 byte = 4 (int) + 2 short
    if (*size != 6){
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

    topic->Data_encode = TopicMarshalling_encode;
    topic->Data_decode = TopicMarshalling_decode;
    topic->domain = 0x05;
    topic->id = 0x07;
    topic->Data_cpy = Data_cpy;

    Locator loc;
    Network_createLocator(&loc);
    // init the msg pool
    for (int i = 0; i < SDDS_TOPIC_APP_MSG_COUNT; i++){
	Msg_init(&(topic->msg.pool[i]), (Data) &(fooBufferPool[i]));
    }

    topic->dsinks.list = loc;

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

