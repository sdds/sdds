#ifdef __cplusplus
extern "C"
{
#endif

#include "sDDS.h"

#ifdef __cplusplus
}
#endif

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED

DDS_Topic g_DCPSParticipant_topic;
Sample_t dcps_participant_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSParticipant dcps_participant_sample_data[SDDS_QOS_HISTORY_DEPTH];
DDS_DataReader g_DCPSParticipant_reader;
DDS_DataWriter g_DCPSParticipant_writer;

DDS_Topic g_DCPSTopic_topic;
Sample_t dcps_topic_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static DDS_DCPSTopic dcps_topic_sample_data[SDDS_QOS_HISTORY_DEPTH];
DDS_DataReader g_DCPSTopic_reader;
DDS_DataWriter g_DCPSTopic_writer;

DDS_Topic g_DCPSPublication_topic;
Sample_t dcps_publication_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static DDS_DCPSPublication dcps_publication_sample_data[SDDS_QOS_HISTORY_DEPTH];
DDS_DataReader g_DCPSPublication_reader;
DDS_DataWriter g_DCPSPublication_writer;

DDS_Topic g_DCPSSubscription_topic;
Sample_t dcps_subscription_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSSubscription dcps_subscription_sample_data[SDDS_QOS_HISTORY_DEPTH];
DDS_DataReader g_DCPSSubscription_reader;
DDS_DataWriter g_DCPSSubscription_writer;

Topic_t*
sDDS_DCPSParticipantTopic_create();
Topic_t*
sDDS_DCPSTopicTopic_create();
Topic_t*
sDDS_DCPSPublicationTopic_create();
Topic_t*
sDDS_DCPSSubscriptionTopic_create();

static uint8_t generalByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t participantByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t subPubByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t topicByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];

/**************
* Initialize *
**************/

rc_t
BuiltinTopic_init(void) {
    int ret;
    Locator_t* l;

    int index;
    for (index = 0; index < SDDS_QOS_HISTORY_DEPTH; index++) {
        dcps_participant_samples_pool[index].data = &dcps_participant_sample_data[index];
        dcps_topic_samples_pool[index].data = &dcps_topic_sample_data[index];
        dcps_publication_samples_pool[index].data = &dcps_publication_sample_data[index];
        dcps_subscription_samples_pool[index].data = &dcps_subscription_sample_data[index];
    }

    BuiltinTopic_participantID = NodeConfig_getNodeID();

    g_DCPSParticipant_topic = sDDS_DCPSParticipantTopic_create();
    g_DCPSParticipant_reader = DataSink_create_datareader(g_DCPSParticipant_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSParticipant_reader), dcps_participant_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSParticipant_writer = DataSource_create_datawriter(g_DCPSParticipant_topic, NULL, NULL, NULL);

    ret = LocatorDB_newMultiLocator(&l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_PARTICIPANT_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    ret = Topic_addRemoteDataSink(g_DCPSParticipant_topic, l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }
    Locator_downRef(l);

    g_DCPSTopic_topic = sDDS_DCPSTopicTopic_create();
    g_DCPSTopic_reader = DataSink_create_datareader(g_DCPSTopic_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSTopic_reader), dcps_topic_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSTopic_writer = DataSource_create_datawriter(g_DCPSTopic_topic, NULL, NULL, NULL);

    ret = LocatorDB_newMultiLocator(&l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_TOPIC_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    ret = Topic_addRemoteDataSink(g_DCPSTopic_topic, l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }
    Locator_downRef(l);

    g_DCPSPublication_topic = sDDS_DCPSPublicationTopic_create();
    g_DCPSPublication_reader = DataSink_create_datareader(g_DCPSPublication_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSPublication_reader), dcps_publication_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSPublication_writer = DataSource_create_datawriter(g_DCPSPublication_topic, NULL, NULL, NULL);

    ret = LocatorDB_newMultiLocator(&l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_SUB_PUB_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    ret = Topic_addRemoteDataSink(g_DCPSPublication_topic, l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }
    Locator_downRef(l);

    g_DCPSSubscription_topic = sDDS_DCPSSubscriptionTopic_create();
    g_DCPSSubscription_reader = DataSink_create_datareader(g_DCPSSubscription_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSSubscription_reader), dcps_subscription_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSSubscription_writer = DataSource_create_datawriter(g_DCPSSubscription_topic, NULL, NULL, NULL);

    ret = LocatorDB_newMultiLocator(&l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    Locator_upRef(l);

    ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_SUB_PUB_ADDRESS);
    if (ret != SDDS_RT_OK) {
        return ret;
    }

    ret = Topic_addRemoteDataSink(g_DCPSSubscription_topic, l);
    if (ret != SDDS_RT_OK) {
        return ret;
    }
    Locator_downRef(l);

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
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

#ifndef FEATURE_SDDS_MULTICAST_ENABLED
    // TO DO
#endif

    return SDDS_RT_OK;
}

/********************
* DCPS Participant *
********************/

rc_t
TopicMarshalling_DCPSParticipant_cpy(Data dest, Data source);

rc_t
TopicMarshalling_DCPSParticipant_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSParticipantDataReader_take_next_sample(
                                               DDS_DataReader _this,
                                               DDS_DCPSParticipant** data_values,
                                               DDS_SampleInfo* sample_info
                                               ) {
    rc_t ret = DataReader_take_next_sample((DataReader_t*) _this, (Data*) data_values, (DataInfo) sample_info);

    if (ret == SDDS_RT_NODATA) {
        return DDS_RETCODE_NO_DATA;
    }

    if (ret == SDDS_RT_OK) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

rc_t
TopicMarshalling_DCPSParticipant_encode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSParticipantDataWriter_write(
                                    DDS_DataWriter _this,
                                    const DDS_DCPSParticipant* instance_data,
                                    const DDS_InstanceHandle_t handle
                                    ) {
    castType_t castType = SDDS_SNPS_CAST_UNICAST;
    addrType_t addrType = SDDS_SNPS_ADDR_IPV6;
    char* addr = "";
    uint8_t addrLen = 0;

#if PLATFORM_LINUX_SDDS_PROTOCOL != AF_INET6
    addrType = SDDS_SNPS_ADDR_IPV4;
#endif

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
    castType = SDDS_SNPS_CAST_MULTICAST;
    addr = subPubByteAddr;
    addrLen = SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE;
#endif

    rc_t ret = DataWriter_writeAddress((DataWriter_t*) _this, castType, addrType, addr, addrLen);
    ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);
    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
        return DDS_RETCODE_OK;
    }
    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSParticipantTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->Data_encode = TopicMarshalling_DCPSParticipant_encode;
    //topic->dsinks.list = locator;

    topic->Data_decode = TopicMarshalling_DCPSParticipant_decode;

    topic->domain = DDS_DCPS_PARTICIPANT_DOMAIN;
    topic->id = DDS_DCPS_PARTICIPANT_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSParticipant_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->dsources.list = List_initConcurrentLinkedList();

    return topic;
}

rc_t
TopicMarshalling_DCPSParticipant_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(SDDS_DCPSParticipant));

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSParticipant_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;

    DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;

    int maxSize = 0;
    maxSize += sizeof(real_data->key);

    byte_t* start = buffer->buff_start + buffer->curPos + 1;

    if ((buffer->curPos + maxSize + 1) >= SDDS_NET_MAX_BUF_SIZE) {
        return SDDS_RT_FAIL;
    }

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSParticipant_decode(NetBuffRef_t* buffer, Data data, size_t* size) {
    DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;

    size_t expectedSize = sizeof(real_data->key);
    if (*size != expectedSize) {
        fprintf(stderr, "%s : size mismatch is %zu should be %zu \n", __FUNCTION__, *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    SNPS_Address_t address;
    rc_t ret = DataSink_getAddr(&address);

    SDDS_DCPSParticipant* sdds_data = (SDDS_DCPSParticipant*) data;

    sdds_data->addr = address.addr;

//    if (address.addrCast == SDDS_SNPS_CAST_UNICAST) {
//        ret = LocatorDB_newLocator(&sdds_data->addr);
//        Locator_upRef(sdds_data->addr);
//        ret = Network_setAddressToLocator(sdds_data->addr, address.addr);
//    }
//    else {
//        ret = LocatorDB_newMultiLocator(&sdds_data->addr);
//        Locator_upRef(sdds_data->addr);
//        ret = Network_setMulticastAddressToLocator(sdds_data->addr, address.addr);
//    }

    return SDDS_RT_OK;
}

/***************
*  DCPS_TOPIC *
***************/

rc_t
TopicMarshalling_DCPSTopic_cpy(Data dest, Data source);

rc_t
TopicMarshalling_DCPSTopic_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSTopicDataReader_take_next_sample(
                                         DDS_DataReader _this,
                                         DDS_DCPSTopic** data_values,
                                         DDS_SampleInfo* sample_info
                                         ) {
    rc_t ret = DataReader_take_next_sample((DataReader_t*) _this, (Data*) data_values, (DataInfo) sample_info);

    if (ret == SDDS_RT_NODATA) {
        return DDS_RETCODE_NO_DATA;
    }

    if (ret == SDDS_RT_OK) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

rc_t
TopicMarshalling_DCPSTopic_encode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSTopicDataWriter_write(
                              DDS_DataWriter _this,
                              const DDS_DCPSTopic* instance_data,
                              const DDS_InstanceHandle_t handle
                              ) {
    rc_t ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);

    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSTopicTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->Data_encode = TopicMarshalling_DCPSTopic_encode;
    topic->Data_decode = TopicMarshalling_DCPSTopic_decode;

    topic->domain = DDS_DCPS_TOPIC_DOMAIN;
    topic->id = DDS_DCPS_TOPIC_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSTopic_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->dsources.list = List_initConcurrentLinkedList();

    return topic;
}

rc_t
TopicMarshalling_DCPSTopic_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(DDS_DCPSTopic));

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSTopic_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;

    DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;

    byte_t* start = buffer->buff_start + buffer->curPos + 1;

    int maxSize = 0;
    maxSize += sizeof(real_data->key);
    maxSize += DDS_TOPIC_NAME_SIZE;

    if ((buffer->curPos + maxSize + 1) >= SDDS_NET_MAX_BUF_SIZE) {
        return SDDS_RT_FAIL;
    }

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_enc_string(start + *size, real_data->name, DDS_TOPIC_NAME_SIZE);
    //*size += sizeof(real_data->name);
    *size += DDS_TOPIC_NAME_SIZE;

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSTopic_decode(NetBuffRef_t* buffer, Data data, size_t* size) {
    DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;

    size_t expectedSize = sizeof(real_data->key) + DDS_TOPIC_NAME_SIZE;
    if (*size != expectedSize) {
        fprintf(stderr, "%s : size mismatch is %zu should be %zu \n", __FUNCTION__, *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_dec_string(start + *size, real_data->name, DDS_TOPIC_NAME_SIZE);
    *size += DDS_TOPIC_NAME_SIZE;

    return SDDS_RT_OK;
}

/********************
* DCPS Publication *
********************/

rc_t
TopicMarshalling_DCPSPublication_cpy(Data dest, Data source);

rc_t
TopicMarshalling_DCPSPublication_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSPublicationDataReader_take_next_sample(
                                               DDS_DataReader _this,
                                               DDS_DCPSPublication** data_values,
                                               DDS_SampleInfo* sample_info
                                               ) {
    rc_t ret = DataReader_take_next_sample((DataReader_t*) _this, (Data*) data_values, (DataInfo) sample_info);

    if (ret == SDDS_RT_NODATA) {
        return DDS_RETCODE_NO_DATA;
    }

    if (ret == SDDS_RT_OK) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

rc_t
TopicMarshalling_DCPSPublication_encode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSPublicationDataWriter_write(
                                    DDS_DataWriter _this,
                                    const DDS_DCPSPublication* instance_data,
                                    const DDS_InstanceHandle_t handle
                                    ) {
    rc_t ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);

    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSPublicationTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->Data_encode = TopicMarshalling_DCPSPublication_encode;
    //topic->dsinks.list = locator;

    topic->Data_decode = TopicMarshalling_DCPSPublication_decode;

    topic->domain = DDS_DCPS_PUBLICATION_DOMAIN;
    topic->id = DDS_DCPS_PUBLICATION_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSPublication_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->dsources.list = List_initConcurrentLinkedList();

    return topic;
}

rc_t
TopicMarshalling_DCPSPublication_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(DDS_DCPSPublication));

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSPublication_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;
    byte_t* start = buffer->buff_start + buffer->curPos + 1;
    DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;

    int maxSize = 0;
    maxSize += sizeof(real_data->key);
    maxSize += sizeof(real_data->participant_key);
    maxSize += sizeof(real_data->topic_id);

    if ((buffer->curPos + maxSize + 1) >= SDDS_NET_MAX_BUF_SIZE) {
        return SDDS_RT_FAIL;
    }

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_enc_uint16(start + *size, &real_data->participant_key);
    *size += sizeof(real_data->participant_key);

    Marshalling_enc_uint16(start + *size, &real_data->topic_id);
    *size += sizeof(real_data->topic_id);

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSPublication_decode(NetBuffRef_t* buffer, Data data, size_t* size) {
    DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;

    size_t expectedSize = sizeof(real_data->key) + sizeof(real_data->participant_key) + sizeof(real_data->topic_id);
    if (*size != expectedSize) {
        fprintf(stderr, "%s : size mismatch is %zu should be %zu \n", __FUNCTION__, *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_dec_uint16(start + *size, &real_data->participant_key);
    *size += sizeof(real_data->participant_key);

    Marshalling_dec_uint16(start + *size, &real_data->topic_id);
    *size += sizeof(real_data->topic_id);

    return SDDS_RT_OK;
}

/*********************
* DCPS Subscription *
*********************/

rc_t
TopicMarshalling_DCPSSubscription_cpy(Data dest, Data source);

rc_t
TopicMarshalling_DCPSSubscription_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSSubscriptionDataReader_take_next_sample(
                                                DDS_DataReader _this,
                                                DDS_DCPSSubscription** data_values,
                                                DDS_SampleInfo* sample_info
                                                ) {
    rc_t ret = DataReader_take_next_sample((DataReader_t*) _this, (Data*) data_values, (DataInfo) sample_info);
    SDDS_DCPSSubscription* data = (SDDS_DCPSSubscription*) *data_values;

    if (ret == SDDS_RT_NODATA) {
        return DDS_RETCODE_NO_DATA;
    }

    if (ret == SDDS_RT_OK) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

rc_t
TopicMarshalling_DCPSSubscription_encode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSSubscriptionDataWriter_write(
                                     DDS_DataWriter _this,
                                     const DDS_DCPSSubscription* instance_data,
                                     const DDS_InstanceHandle_t handle
                                     ) {
    castType_t castType = SDDS_SNPS_CAST_UNICAST;
    addrType_t addrType = SDDS_SNPS_ADDR_IPV6;
    char* addr = "";
    uint8_t addrLen = 0;

#if PLATFORM_LINUX_SDDS_PROTOCOL != AF_INET6
    addrType = SDDS_SNPS_ADDR_IPV4;
#endif

#ifdef FEATURE_SDDS_MULTICAST_ENABLED
    castType = SDDS_SNPS_CAST_MULTICAST;
    addr = generalByteAddr;
    addrLen = SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE;
#endif

    rc_t ret = DataWriter_writeAddress((DataWriter_t*) _this, castType, addrType, addr, addrLen);
    ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);

    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSSubscriptionTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->Data_encode = TopicMarshalling_DCPSSubscription_encode;

    topic->Data_decode = TopicMarshalling_DCPSSubscription_decode;

    topic->domain = DDS_DCPS_SUBSCRIPTION_DOMAIN;
    topic->id = DDS_DCPS_SUBSCRIPTION_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSSubscription_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->dsources.list = List_initConcurrentLinkedList();

    return topic;
}

rc_t
TopicMarshalling_DCPSSubscription_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(SDDS_DCPSSubscription));
    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSSubscription_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;

    DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;

    int maxSize = 0;
    maxSize += sizeof(real_data->key);
    maxSize += sizeof(real_data->participant_key);
    maxSize += sizeof(real_data->topic_id);

    if ((buffer->curPos + maxSize + 1) >= SDDS_NET_MAX_BUF_SIZE) {
        return SDDS_RT_FAIL;
    }

    byte_t* start = buffer->buff_start + buffer->curPos + 1;

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_enc_uint16(start + *size, &real_data->participant_key);
    *size += sizeof(real_data->participant_key);

    Marshalling_enc_uint16(start + *size, &real_data->topic_id);
    *size += sizeof(real_data->topic_id);

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSSubscription_decode(NetBuffRef_t* buffer, Data data, size_t* size) {
    DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;

    size_t expectedSize = sizeof(real_data->key) + sizeof(real_data->participant_key) + sizeof(real_data->topic_id);
    if (*size != expectedSize) {
        fprintf(stderr, "%s : size mismatch is %zu should be %zu \n", __FUNCTION__, *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_dec_uint16(start + *size, &real_data->participant_key);
    *size += sizeof(real_data->participant_key);

    Marshalling_dec_uint16(start + *size, &real_data->topic_id);
    *size += sizeof(real_data->topic_id);

    SNPS_Address_t address;
    rc_t ret = DataSink_getAddr(&address);

    SDDS_DCPSSubscription* sdds_data = (SDDS_DCPSSubscription*) data;

    sdds_data->addr = address.addr;

//    if (address.addrCast == SDDS_SNPS_CAST_UNICAST) {
//        ret = LocatorDB_newLocator(&sdds_data->addr);
//        Locator_upRef(sdds_data->addr);
//        ret = Network_setAddressToLocator(sdds_data->addr, address.addr);
//    }
//    else {
//        ret = LocatorDB_newMultiLocator(&sdds_data->addr);
//        Locator_upRef(sdds_data->addr);
//        ret = Network_setMulticastAddressToLocator(sdds_data->addr, address.addr);
//    }

    return SDDS_RT_OK;
}

bool
BuildinTopic_isBuiltinTopic(topicid_t tID, domainid_t dID) {
    if (
        (DDS_DCPS_PARTICIPANT_DOMAIN == dID) && (DDS_DCPS_PARTICIPANT_TOPIC == tID)
       || (DDS_DCPS_TOPIC_DOMAIN == dID) && (DDS_DCPS_TOPIC_TOPIC == tID)
       || (DDS_DCPS_PUBLICATION_DOMAIN == dID) && (DDS_DCPS_PUBLICATION_TOPIC == tID)
       || (DDS_DCPS_SUBSCRIPTION_DOMAIN == dID) && (DDS_DCPS_SUBSCRIPTION_TOPIC == tID)
        ) {
        return true;
    }

    return false;
}

#endif
