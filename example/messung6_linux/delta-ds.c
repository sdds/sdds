#ifdef __cplusplus
extern "C"
{
#endif

#include "delta-ds.h"

#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Marshalling.h>
#include <sdds/Network.h>
#include <sdds/TopicDB.h>
#include <sdds/TopicMarshalling.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#define SDDS_DOMAIN 0x1
#define SDDS_TOPIC 0x13


rc_t TopicMarshalling_Delta_cpy(Data dest, Data source);

#ifdef SDDS_TOPIC_HAS_PUB

rc_t TopicMarshalling_Delta_decode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DeltaDataReader_take_next_sample(
	DDS_DataReader _this,
	Delta** data_values,
	DDS_SampleInfo* sample_info
)
{
	rc_t ret = DataSink_take_next_sample((DataReader) _this, (Data*) data_values, (DataInfo) sample_info);

	if (ret == SDDS_RT_NODATA)
		return DDS_RETCODE_NO_DATA;

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
/*
DDS_ReturnCode_t DDS_DeltaDataReader_set_listener(
	DDS_DataReader _this,
	const struct DDS_DataReaderListener* a_listener,
	const DDS_StatusMask mask
)
{
	rc_t ret = DataSink_set_on_data_avail_listener((DataReader) _this, (On_Data_Avail_Listener) a_listener->on_data_available, (const StatusMask) mask);
	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
*/
#endif

#ifdef SDDS_TOPIC_HAS_SUB

rc_t TopicMarshalling_Delta_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DeltaDataWriter_write(
	DDS_DataWriter _this,
	const Delta* instance_data,
	const DDS_InstanceHandle_t  handle
)
{
	rc_t ret = DataSource_write((DataWriter) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
#endif

Topic sDDS_DeltaTopic_create(Delta* pool, int count)
{
	Topic topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));

#ifdef SDDS_TOPIC_HAS_SUB
	topic->Data_encode = TopicMarshalling_Delta_encode;
	//topic->dsinks.list = locator;
#endif

#ifdef SDDS_TOPIC_HAS_PUB
	topic->Data_decode = TopicMarshalling_Delta_decode;
#endif

	topic->domain = SDDS_DOMAIN;
	topic->id = SDDS_TOPIC;
	topic->Data_cpy = TopicMarshalling_Delta_cpy;

	return topic;
}

rc_t TopicMarshalling_Delta_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(Delta));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_Delta_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	Delta* real_data = (Delta*) data;

	Marshalling_enc_int8(buffer + *size, &real_data->value);
	*size += sizeof(real_data->value);


	return SDDS_RT_OK;
}

rc_t TopicMarshalling_Delta_decode(byte_t* buffer, Data data, size_t* size)
{
	if (*size != 1)
		fprintf(stderr, "%s : size mismatch is %d should be %d \n",__FUNCTION__, *size, 1);

	*size = 0;

	Delta* real_data = (Delta*) data;

	Marshalling_dec_int8(buffer + *size, &real_data->value);
	*size += sizeof(real_data->value);

	return SDDS_RT_OK;
}
