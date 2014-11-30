#ifdef __cplusplus
extern "C"
{
#endif

#include "BuiltinTopic.h"

#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Marshalling.h>
#include <sdds/Network.h>
#include <sdds/TopicDB.h>
#include <sdds/TopicMarshalling.h>
#include <sdds/sDDS.h>
#include <sdds/Log.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

DDS_Topic g_DCPSParticipant_topic;
DDS_DCPSParticipant g_DCPSParticipant_pool[sDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSParticipant_reader;
DDS_DataWriter g_DCPSParticipant_writer;

DDS_Topic g_DCPSTopic_topic;
DDS_DCPSTopic g_DCPSTopic_pool[sDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSTopic_reader;
DDS_DataWriter g_DCPSTopic_writer;

DDS_Topic g_DCPSPublication_topic;
DDS_DCPSPublication g_DCPSPublication_pool[sDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSPublication_reader;
DDS_DataWriter g_DCPSPublication_writer;

DDS_Topic g_DCPSSubscription_topic;
DDS_DCPSSubscription g_DCPSSubscription_pool[sDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSSubscription_reader;
DDS_DataWriter g_DCPSSubscription_writer;


Topic sDDS_DCPSParticipantTopic_create(DDS_DCPSParticipant* pool, int count);
Topic sDDS_DCPSTopicTopic_create(DDS_DCPSTopic* pool, int count);
Topic sDDS_DCPSPublicationTopic_create(DDS_DCPSPublication* pool, int count);
Topic sDDS_DCPSSubscriptionTopic_create(DDS_DCPSSubscription* pool, int count);

/**************
 * Initialize *
 **************/

rc_t BuiltinTopic_init(void) 
{

    int ret;

    Locator l;

	g_DCPSParticipant_topic = sDDS_DCPSParticipantTopic_create(g_DCPSParticipant_pool, sDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSParticipant_reader = DataSink_create_datareader(g_DCPSParticipant_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSParticipant_writer = DataSource_create_datawriter(g_DCPSParticipant_topic, NULL, NULL, NULL);


	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, "ff02::01:10");
      if (ret != SDDS_RT_OK)
         return ret;

	ret = Topic_addRemoteDataSink(g_DCPSParticipant_topic, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	g_DCPSTopic_topic = sDDS_DCPSTopicTopic_create(g_DCPSTopic_pool, sDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSTopic_reader = DataSink_create_datareader(g_DCPSTopic_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSTopic_writer = DataSource_create_datawriter(g_DCPSTopic_topic, NULL, NULL, NULL);


	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, "ff02::01:10");
      if (ret != SDDS_RT_OK)
         return ret;


	ret = Topic_addRemoteDataSink(g_DCPSTopic_topic, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	g_DCPSPublication_topic = sDDS_DCPSPublicationTopic_create(g_DCPSPublication_pool, sDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSPublication_reader = DataSink_create_datareader(g_DCPSPublication_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSPublication_writer = DataSource_create_datawriter(g_DCPSPublication_topic, NULL, NULL, NULL);


	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, "ff02::01:10");
      if (ret != SDDS_RT_OK)
         return ret;


	ret = Topic_addRemoteDataSink(g_DCPSPublication_topic, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	g_DCPSSubscription_topic = sDDS_DCPSSubscriptionTopic_create(g_DCPSSubscription_pool, sDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSSubscription_reader = DataSink_create_datareader(g_DCPSSubscription_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSSubscription_writer = DataSource_create_datawriter(g_DCPSSubscription_topic, NULL, NULL, NULL);


	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
		return ret;

	Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, "ff02::01:10");
      if (ret != SDDS_RT_OK)
         return ret;

	ret = Topic_addRemoteDataSink(g_DCPSSubscription_topic, l);
	if (ret != SDDS_RT_OK)
		return ret;
	Locator_downRef(l);

	return SDDS_RT_OK;
}


/********************
 * DCPS Participant *
 ********************/

rc_t TopicMarshalling_DCPSParticipant_cpy(Data dest, Data source);

rc_t TopicMarshalling_DCPSParticipant_decode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSParticipantDataReader_take_next_sample(
	DDS_DataReader _this,
	DDS_DCPSParticipant** data_values,
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
DDS_ReturnCode_t DDS_DCPSParticipantDataReader_set_listener(
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

rc_t TopicMarshalling_DCPSParticipant_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSParticipantDataWriter_write(
	DDS_DataWriter _this,
	const DDS_DCPSParticipant* instance_data,
	const DDS_InstanceHandle_t  handle
)
{
	rc_t ret = DataSource_write((DataWriter) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic sDDS_DCPSParticipantTopic_create(DDS_DCPSParticipant* pool, int count)
{
	Topic topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));


	topic->Data_encode = TopicMarshalling_DCPSParticipant_encode;
	//topic->dsinks.list = locator;

	topic->Data_decode = TopicMarshalling_DCPSParticipant_decode;

	topic->domain = DCPS_PARTICIPANT_DOMAIN;
	topic->id = DCPS_PARTICIPANT_TOPIC;
	topic->Data_cpy = TopicMarshalling_DCPSParticipant_cpy;

	return topic;
}

rc_t TopicMarshalling_DCPSParticipant_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(DDS_DCPSParticipant));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSParticipant_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;

	Marshalling_enc_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);


	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSParticipant_decode(byte_t* buffer, Data data, size_t* size)
{
	if (*size != 1)
		fprintf(stderr, "%s : size mismatch is %d should be %d \n",__FUNCTION__, *size, 1);

	*size = 0;

	DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;

	Marshalling_dec_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	return SDDS_RT_OK;
}

/***************
 *  DCPS_TOPIC *
 ***************/

rc_t TopicMarshalling_DCPSTopic_cpy(Data dest, Data source);



rc_t TopicMarshalling_DCPSTopic_decode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSTopicDataReader_take_next_sample(
	DDS_DataReader _this,
	DDS_DCPSTopic** data_values,
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
DDS_ReturnCode_t DDS_DCPSTopicDataReader_set_listener(
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



rc_t TopicMarshalling_DCPSTopic_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSTopicDataWriter_write(
	DDS_DataWriter _this,
	const DDS_DCPSTopic* instance_data,
	const DDS_InstanceHandle_t  handle
)
{
	rc_t ret = DataSource_write((DataWriter) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic sDDS_DCPSTopicTopic_create(DDS_DCPSTopic* pool, int count)
{
	Topic topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));


	topic->Data_encode = TopicMarshalling_DCPSTopic_encode;
	//topic->dsinks.list = locator;


	topic->Data_decode = TopicMarshalling_DCPSTopic_decode;

	topic->domain = DCPS_TOPIC_DOMAIN;
	topic->id = DCPS_TOPIC_TOPIC;
	topic->Data_cpy = TopicMarshalling_DCPSTopic_cpy;

	return topic;
}

rc_t TopicMarshalling_DCPSTopic_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(DDS_DCPSTopic));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSTopic_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;

	Marshalling_enc_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_enc_string(buffer + *size, real_data->name, TOPIC_NAME_SIZE);
	*size += sizeof(real_data->name);

	Marshalling_enc_string(buffer + *size, real_data->type_name, TOPIC_TYPE_SIZE);
	*size += sizeof(real_data->type_name);


	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSTopic_decode(byte_t* buffer, Data data, size_t* size)
{
    int expectedSize = 1 + TOPIC_NAME_SIZE + TOPIC_TYPE_SIZE;
	if (*size != expectedSize)
		fprintf(stderr, "%s : size mismatch is %d should be %d \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;

	Marshalling_dec_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_dec_string(buffer + *size, real_data->name, TOPIC_NAME_SIZE);
	*size += sizeof(real_data->name);

	Marshalling_dec_string(buffer + *size, real_data->type_name, TOPIC_TYPE_SIZE);
	*size += sizeof(real_data->type_name);

	return SDDS_RT_OK;
}

/********************
 * DCPS Publication *
 ********************/

rc_t TopicMarshalling_DCPSPublication_cpy(Data dest, Data source);

rc_t TopicMarshalling_DCPSPublication_decode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSPublicationDataReader_take_next_sample(
	DDS_DataReader _this,
	DDS_DCPSPublication** data_values,
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
DDS_ReturnCode_t DDS_DCPSPublicationDataReader_set_listener(
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


rc_t TopicMarshalling_DCPSPublication_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSPublicationDataWriter_write(
	DDS_DataWriter _this,
	const DDS_DCPSPublication* instance_data,
	const DDS_InstanceHandle_t  handle
)
{
	rc_t ret = DataSource_write((DataWriter) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic sDDS_DCPSPublicationTopic_create(DDS_DCPSPublication* pool, int count)
{
	Topic topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));


	topic->Data_encode = TopicMarshalling_DCPSPublication_encode;
	//topic->dsinks.list = locator;


	topic->Data_decode = TopicMarshalling_DCPSPublication_decode;

	topic->domain = DCPS_PUBLICATION_DOMAIN;
	topic->id = DCPS_PUBLICATION_TOPIC;
	topic->Data_cpy = TopicMarshalling_DCPSPublication_cpy;

	return topic;
}

rc_t TopicMarshalling_DCPSPublication_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(DDS_DCPSPublication));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSPublication_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;

	Marshalling_enc_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_enc_uint8(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_enc_string(buffer + *size, real_data->topic_name, TOPIC_NAME_SIZE);
	*size += sizeof(real_data->topic_name);

	Marshalling_enc_string(buffer + *size, real_data->type_name, TOPIC_TYPE_SIZE);
	*size += sizeof(real_data->type_name);


	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSPublication_decode(byte_t* buffer, Data data, size_t* size)
{
    int expectedSize = 2 + TOPIC_NAME_SIZE + TOPIC_TYPE_SIZE;
	if (*size != expectedSize)
		fprintf(stderr, "%s : size mismatch is %d should be %d \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;

	Marshalling_dec_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_dec_uint8(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_dec_string(buffer + *size, real_data->topic_name, TOPIC_NAME_SIZE);
	*size += sizeof(real_data->topic_name);

	Marshalling_dec_string(buffer + *size, real_data->type_name, TOPIC_TYPE_SIZE);
	*size += sizeof(real_data->type_name);

	return SDDS_RT_OK;
}

/*********************
 * DCPS Subscription *
 *********************/

rc_t TopicMarshalling_DCPSSubscription_cpy(Data dest, Data source);

rc_t TopicMarshalling_DCPSSubscription_decode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSSubscriptionDataReader_take_next_sample(
	DDS_DataReader _this,
	DDS_DCPSSubscription** data_values,
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
DDS_ReturnCode_t DDS_DCPSSubscriptionDataReader_set_listener(
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


rc_t TopicMarshalling_DCPSSubscription_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSSubscriptionDataWriter_write(
	DDS_DataWriter _this,
	const DDS_DCPSSubscription* instance_data,
	const DDS_InstanceHandle_t  handle
)
{
	rc_t ret = DataSource_write((DataWriter) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic sDDS_DCPSSubscriptionTopic_create(DDS_DCPSSubscription* pool, int count)
{
	Topic topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));


	topic->Data_encode = TopicMarshalling_DCPSSubscription_encode;
	//topic->dsinks.list = locator;

	topic->Data_decode = TopicMarshalling_DCPSSubscription_decode;

	topic->domain = DCPS_SUBSCRIPTION_DOMAIN;
	topic->id = DCPS_SUBSCRIPTION_TOPIC;
	topic->Data_cpy = TopicMarshalling_DCPSSubscription_cpy;

	return topic;
}

rc_t TopicMarshalling_DCPSSubscription_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(DDS_DCPSSubscription));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSSubscription_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;

	Marshalling_enc_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_enc_uint8(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_enc_string(buffer + *size, real_data->topic_name, TOPIC_NAME_SIZE);
	*size += sizeof(real_data->topic_name);

	Marshalling_enc_string(buffer + *size, real_data->type_name, TOPIC_TYPE_SIZE);
	*size += sizeof(real_data->type_name);


	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSSubscription_decode(byte_t* buffer, Data data, size_t* size)
{
    int expectedSize = 2 + TOPIC_NAME_SIZE + TOPIC_TYPE_SIZE;
	if (*size != expectedSize)
		fprintf(stderr, "%s : size mismatch is %d should be %d \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;

	Marshalling_dec_uint8(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_dec_uint8(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_dec_string(buffer + *size, real_data->topic_name, TOPIC_NAME_SIZE);
	*size += sizeof(real_data->topic_name);

	Marshalling_dec_string(buffer + *size, real_data->type_name, TOPIC_TYPE_SIZE);
	*size += sizeof(real_data->type_name);

	return SDDS_RT_OK;
}
