#ifdef __cplusplus
extern "C"
{
#endif

#include "BuiltinTopic.h"
#include "DataSource.h"
#include "DataSink.h"
#include "SNPS.h"

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

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED

DDS_Topic g_DCPSParticipant_topic;
SDDS_DCPSParticipant g_DCPSParticipant_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSParticipant_reader;
DDS_DataWriter g_DCPSParticipant_writer;

DDS_Topic g_DCPSTopic_topic;
DDS_DCPSTopic g_DCPSTopic_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSTopic_reader;
DDS_DataWriter g_DCPSTopic_writer;

DDS_Topic g_DCPSPublication_topic;
DDS_DCPSPublication g_DCPSPublication_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSPublication_reader;
DDS_DataWriter g_DCPSPublication_writer;

DDS_Topic g_DCPSSubscription_topic;
SDDS_DCPSSubscription g_DCPSSubscription_pool[SDDS_TOPIC_APP_MSG_COUNT];
DDS_DataReader g_DCPSSubscription_reader;
DDS_DataWriter g_DCPSSubscription_writer;

Topic_t * sDDS_DCPSParticipantTopic_create(SDDS_DCPSParticipant* pool, int count);
Topic_t * sDDS_DCPSTopicTopic_create(DDS_DCPSTopic* pool, int count);
Topic_t * sDDS_DCPSPublicationTopic_create(DDS_DCPSPublication* pool, int count);
Topic_t * sDDS_DCPSSubscriptionTopic_create(SDDS_DCPSSubscription* pool, int count);

static uint8_t generalByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t participantByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t subPubByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t topicByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];

/**************
 * Initialize *
 **************/

void BuiltinTopic_printSubPool() {
	for (int i = 0; i < SDDS_TOPIC_APP_MSG_COUNT; i++) {
		printf("-----------------------> TEST.topic_id %d\n", g_DCPSSubscription_pool[i].data.topic_id);
		printf("-----------------------> TEST.addr %p\n", g_DCPSSubscription_pool[i].addr);
	}
}

void BuiltinTopic_printTopicAddr() {
	printf("---------> g_DCPSParticipant_topic %p\n", g_DCPSParticipant_topic);
	printf("---------> g_DCPSPublication_topic %p\n", g_DCPSPublication_topic);
	printf("---------> g_DCPSSubscription_topic %p\n", g_DCPSSubscription_topic);
	printf("---------> g_DCPSTopic_topic %p\n", g_DCPSTopic_topic);
}

rc_t BuiltinTopic_init(void)
{
	int ret;
	Locator l;

	BuiltinTopic_participantID = NodeConfig_getNodeID();

	g_DCPSParticipant_topic = sDDS_DCPSParticipantTopic_create(g_DCPSParticipant_pool, SDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSParticipant_reader = DataSink_create_datareader(g_DCPSParticipant_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSParticipant_writer = DataSource_create_datawriter(g_DCPSParticipant_topic, NULL, NULL, NULL);

	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
	return ret;

	Locator_upRef(l);

	ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_PARTICIPANT_ADDRESS);
	if (ret != SDDS_RT_OK) {
		return ret;
	}

	ret = Topic_addRemoteDataSink(g_DCPSParticipant_topic, l);
	if (ret != SDDS_RT_OK)
	return ret;
	Locator_downRef(l);

	g_DCPSTopic_topic = sDDS_DCPSTopicTopic_create(g_DCPSTopic_pool, SDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSTopic_reader = DataSink_create_datareader(g_DCPSTopic_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSTopic_writer = DataSource_create_datawriter(g_DCPSTopic_topic, NULL, NULL, NULL);

	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
	return ret;

	Locator_upRef(l);

	ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_TOPIC_ADDRESS);
	if (ret != SDDS_RT_OK)
	return ret;

	ret = Topic_addRemoteDataSink(g_DCPSTopic_topic, l);
	if (ret != SDDS_RT_OK)
	return ret;
	Locator_downRef(l);

	g_DCPSPublication_topic = sDDS_DCPSPublicationTopic_create(g_DCPSPublication_pool, SDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSPublication_reader = DataSink_create_datareader(g_DCPSPublication_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSPublication_writer = DataSource_create_datawriter(g_DCPSPublication_topic, NULL, NULL, NULL);

	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
	return ret;

	Locator_upRef(l);

	ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_SUB_PUB_ADDRESS);
	if (ret != SDDS_RT_OK)
	return ret;

	ret = Topic_addRemoteDataSink(g_DCPSPublication_topic, l);
	if (ret != SDDS_RT_OK)
	return ret;
	Locator_downRef(l);

	g_DCPSSubscription_topic = sDDS_DCPSSubscriptionTopic_create(g_DCPSSubscription_pool, SDDS_TOPIC_APP_MSG_COUNT);
	g_DCPSSubscription_reader = DataSink_create_datareader(g_DCPSSubscription_topic, NULL, NULL /*&sdds_listener*/, NULL);
	g_DCPSSubscription_writer = DataSource_create_datawriter(g_DCPSSubscription_topic, NULL, NULL, NULL);

	ret = LocatorDB_newMultiLocator(&l);
	if (ret != SDDS_RT_OK)
	return ret;

	Locator_upRef(l);

	ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_SUB_PUB_ADDRESS);
	if (ret != SDDS_RT_OK)
	return ret;

	ret = Topic_addRemoteDataSink(g_DCPSSubscription_topic, l);
	if (ret != SDDS_RT_OK)
	return ret;
	Locator_downRef(l);

#ifdef _MULTICAST
	uint8_t addrLen;
	ret = SNPS_IPv6_str2Addr(SDDS_BUILTIN_MULTICAST_ADDRESS, generalByteAddr, &addrLen);
	if (ret != SDDS_RT_OK) {
		return ret;
	}
	ret = SNPS_IPv6_str2Addr(SDDS_BUILTIN_PARTICIPANT_ADDRESS, participantByteAddr, &addrLen);
	if (ret != SDDS_RT_OK) {
		return ret;
	}
	ret = SNPS_IPv6_str2Addr(SDDS_BUILTIN_SUB_PUB_ADDRESS, subPubByteAddr, &addrLen);
	if (ret != SDDS_RT_OK) {
		return ret;
	}
	ret = SNPS_IPv6_str2Addr(SDDS_BUILTIN_TOPIC_ADDRESS, topicByteAddr, &addrLen);
	if (ret != SDDS_RT_OK) {
		return ret;
	}
#endif

#ifndef _MULTICAST
	// TO DO
#endif

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
	rc_t ret = DataReader_take_next_sample((DataReader_t *) _this, (Data*) data_values, (DataInfo) sample_info);

	if (ret == SDDS_RT_NODATA)
	return DDS_RETCODE_NO_DATA;

	if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

rc_t TopicMarshalling_DCPSParticipant_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSParticipantDataWriter_write(
		DDS_DataWriter _this,
		const DDS_DCPSParticipant* instance_data,
		const DDS_InstanceHandle_t handle
)
{
	castType_t castType = SDDS_SNPS_CAST_UNICAST;
	addrType_t addrType = SDDS_SNPS_ADDR_IPV6;
	char *addr = "";
	uint8_t addrLen = 0;

#if PLATFORM_LINUX_SDDS_PROTOCOL != AF_INET6
		addrType = SDDS_SNPS_ADDR_IPV4;
#endif

#ifdef _MULTICAST
		castType = SDDS_SNPS_CAST_MULTICAST;
		addr = subPubByteAddr;
		addrLen = SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE;
#endif

	rc_t ret = DataSource_writeAddress((DataWriter_t *) _this, castType, addrType, addr, addrLen);
	ret = DataSource_write((DataWriter_t *) _this, (Data)instance_data, (void*) handle);
	if (ret == SDDS_RT_OK) {
		return DDS_RETCODE_OK;
	}
	return DDS_RETCODE_ERROR;
}

Topic_t * sDDS_DCPSParticipantTopic_create(SDDS_DCPSParticipant* pool, int count)
{
	Topic_t *topic = TopicDB_createTopic();

	for (int i = 0; i < count; i++) {
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));
	}

	topic->Data_encode = TopicMarshalling_DCPSParticipant_encode;
	//topic->dsinks.list = locator;

	topic->Data_decode = TopicMarshalling_DCPSParticipant_decode;

	topic->domain = DDS_DCPS_PARTICIPANT_DOMAIN;
	topic->id = DDS_DCPS_PARTICIPANT_TOPIC;
	topic->Data_cpy = TopicMarshalling_DCPSParticipant_cpy;

	return topic;
}

rc_t TopicMarshalling_DCPSParticipant_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(SDDS_DCPSParticipant));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSParticipant_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;

	Marshalling_enc_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSParticipant_decode(byte_t* buffer, Data data, size_t* size)
{
	DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;

	size_t expectedSize = sizeof(real_data->key);
	if (*size != expectedSize)
	fprintf(stderr, "%s : size mismatch is %zu should be %zu \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	Marshalling_dec_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	SNPS_Address_t address;
	rc_t ret = DataSink_getAddr(&address);

	SDDS_DCPSParticipant* sdds_data = (SDDS_DCPSParticipant*) data;

	if (address.addrCast == SDDS_SNPS_CAST_UNICAST) {
		ret = LocatorDB_newLocator(&sdds_data->addr);
		Locator_upRef(sdds_data->addr);
		ret = Network_setAddressToLocator(sdds_data->addr, address.addr);
	}
	else {
		ret = LocatorDB_newMultiLocator(&sdds_data->addr);
		Locator_upRef(sdds_data->addr);
		ret = Network_setMulticastAddressToLocator(sdds_data->addr, address.addr);
	}

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
	rc_t ret = DataReader_take_next_sample((DataReader_t *) _this, (Data*) data_values, (DataInfo) sample_info);

	if (ret == SDDS_RT_NODATA)
	return DDS_RETCODE_NO_DATA;

	if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

rc_t TopicMarshalling_DCPSTopic_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSTopicDataWriter_write(
		DDS_DataWriter _this,
		const DDS_DCPSTopic* instance_data,
		const DDS_InstanceHandle_t handle
)
{
	rc_t ret = DataSource_write((DataWriter_t *) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic_t * sDDS_DCPSTopicTopic_create(DDS_DCPSTopic* pool, int count)
{
	Topic_t *topic = TopicDB_createTopic();

	for (int i = 0; i < count; i++) {
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));
	}

	topic->Data_encode = TopicMarshalling_DCPSTopic_encode;
	topic->Data_decode = TopicMarshalling_DCPSTopic_decode;

	topic->domain = DDS_DCPS_TOPIC_DOMAIN;
	topic->id = DDS_DCPS_TOPIC_TOPIC;
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

	printf("%d %s\n", __LINE__, __FUNCTION__);

	Marshalling_enc_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_enc_string(buffer + *size, real_data->name, DDS_TOPIC_NAME_SIZE);
	//*size += sizeof(real_data->name);
	*size += DDS_TOPIC_NAME_SIZE;

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSTopic_decode(byte_t* buffer, Data data, size_t* size)
{
	DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;

	size_t expectedSize = sizeof(real_data->key) + DDS_TOPIC_NAME_SIZE;
	if (*size != expectedSize)
	fprintf(stderr, "%s : size mismatch is %zu should be %zu \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	Marshalling_dec_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_dec_string(buffer + *size, real_data->name, DDS_TOPIC_NAME_SIZE);
	*size += DDS_TOPIC_NAME_SIZE;

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
	rc_t ret = DataReader_take_next_sample((DataReader_t *) _this, (Data*) data_values, (DataInfo) sample_info);

	if (ret == SDDS_RT_NODATA)
	return DDS_RETCODE_NO_DATA;

	if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

rc_t TopicMarshalling_DCPSPublication_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSPublicationDataWriter_write(
		DDS_DataWriter _this,
		const DDS_DCPSPublication* instance_data,
		const DDS_InstanceHandle_t handle
)
{
	rc_t ret = DataSource_write((DataWriter_t *) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic_t * sDDS_DCPSPublicationTopic_create(DDS_DCPSPublication* pool, int count)
{
	Topic_t *topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
	Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));

	topic->Data_encode = TopicMarshalling_DCPSPublication_encode;
	//topic->dsinks.list = locator;

	topic->Data_decode = TopicMarshalling_DCPSPublication_decode;

	topic->domain = DDS_DCPS_PUBLICATION_DOMAIN;
	topic->id = DDS_DCPS_PUBLICATION_TOPIC;
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

	Marshalling_enc_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_enc_uint16(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_enc_uint16(buffer + *size, &real_data->topic_id);
	*size += sizeof(real_data->topic_id);

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSPublication_decode(byte_t* buffer, Data data, size_t* size)
{
	DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;

	size_t expectedSize = sizeof(real_data->key) + sizeof(real_data->participant_key) + sizeof(real_data->topic_id);
	if (*size != expectedSize)
	fprintf(stderr, "%s : size mismatch is %zu should be %zu \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	Marshalling_dec_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_dec_uint16(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_dec_uint16(buffer + *size, &real_data->topic_id);
	*size += sizeof(real_data->topic_id);

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
	rc_t ret = DataReader_take_next_sample((DataReader_t *) _this, (Data*) data_values, (DataInfo) sample_info);

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
 rc_t ret = DataSink_set_on_data_avail_listener((DataReader_t *) _this, (On_Data_Avail_Listener) a_listener->on_data_available, (const StatusMask) mask);
 if (ret == SDDS_RT_OK)
 return DDS_RETCODE_OK;

 return DDS_RETCODE_ERROR;
 }
 */

rc_t TopicMarshalling_DCPSSubscription_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_DCPSSubscriptionDataWriter_write(
		DDS_DataWriter _this,
		const DDS_DCPSSubscription* instance_data,
		const DDS_InstanceHandle_t handle
)
{
	castType_t castType = SDDS_SNPS_CAST_UNICAST;
	addrType_t addrType = SDDS_SNPS_ADDR_IPV6;
	char *addr = "";
	uint8_t addrLen = 0;

#if PLATFORM_LINUX_SDDS_PROTOCOL != AF_INET6
		addrType = SDDS_SNPS_ADDR_IPV4;
#endif

#ifdef _MULTICAST
		castType = SDDS_SNPS_CAST_MULTICAST;
		addr = generalByteAddr;
		addrLen = SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE;
#endif

	rc_t ret = DataSource_writeAddress((DataWriter_t *) _this, castType, addrType, addr, addrLen);
	ret = DataSource_write((DataWriter_t *) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
	return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

Topic_t * sDDS_DCPSSubscriptionTopic_create(SDDS_DCPSSubscription* pool, int count)
{
	Topic_t *topic = TopicDB_createTopic();

	for (int i = 0; i < count; i++) {
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));
	}

	topic->Data_encode = TopicMarshalling_DCPSSubscription_encode;

	topic->Data_decode = TopicMarshalling_DCPSSubscription_decode;

	topic->domain = DDS_DCPS_SUBSCRIPTION_DOMAIN;
	topic->id = DDS_DCPS_SUBSCRIPTION_TOPIC;
	topic->Data_cpy = TopicMarshalling_DCPSSubscription_cpy;

	return topic;
}

rc_t TopicMarshalling_DCPSSubscription_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(SDDS_DCPSSubscription));
	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSSubscription_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;

	Marshalling_enc_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_enc_uint16(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_enc_uint16(buffer + *size, &real_data->topic_id);
	*size += sizeof(real_data->topic_id);

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_DCPSSubscription_decode(byte_t* buffer, Data data, size_t* size)
{
	DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;

	size_t expectedSize = sizeof(real_data->key) + sizeof(real_data->participant_key) + sizeof(real_data->topic_id);
	if (*size != expectedSize)
	fprintf(stderr, "%s : size mismatch is %zu should be %zu \n",__FUNCTION__, *size, expectedSize);

	*size = 0;

	Marshalling_dec_uint16(buffer + *size, &real_data->key);
	*size += sizeof(real_data->key);

	Marshalling_dec_uint16(buffer + *size, &real_data->participant_key);
	*size += sizeof(real_data->participant_key);

	Marshalling_dec_uint16(buffer + *size, &real_data->topic_id);
	*size += sizeof(real_data->topic_id);

	SNPS_Address_t address;
	rc_t ret = DataSink_getAddr(&address);

	SDDS_DCPSSubscription* sdds_data = (SDDS_DCPSSubscription*) data;

	if (address.addrCast == SDDS_SNPS_CAST_UNICAST) {
		ret = LocatorDB_newLocator(&sdds_data->addr);
		Locator_upRef(sdds_data->addr);
		ret = Network_setAddressToLocator(sdds_data->addr, address.addr);
	}
	else {
		ret = LocatorDB_newMultiLocator(&sdds_data->addr);
		Locator_upRef(sdds_data->addr);
		ret = Network_setMulticastAddressToLocator(sdds_data->addr, address.addr);
	}

	return SDDS_RT_OK;
}

bool BuildinTopic_isBuiltinTopic(topicid_t tID, domainid_t dID) {
	if (
			(DDS_DCPS_PARTICIPANT_DOMAIN == dID) && (DDS_DCPS_PARTICIPANT_TOPIC == tID) ||
			(DDS_DCPS_TOPIC_DOMAIN == dID) && (DDS_DCPS_TOPIC_TOPIC == tID) ||
			(DDS_DCPS_PUBLICATION_DOMAIN == dID) && (DDS_DCPS_PUBLICATION_TOPIC == tID) ||
			(DDS_DCPS_SUBSCRIPTION_DOMAIN == dID) && (DDS_DCPS_SUBSCRIPTION_TOPIC == tID)
	) {
		return true;
	}

	return false;
}

#endif
