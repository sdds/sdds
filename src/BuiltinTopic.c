#ifdef __cplusplus
extern "C"
{
#endif

#include "sDDS.h"

#ifdef FEATURE_SDDS_MANAGEMENT_TOPIC_ENABLED
#include "ManagementTopic.h"
#endif

#ifdef __cplusplus
}
#endif

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED

SSW_NodeID_t BuiltinTopic_participantID;

#ifdef TEST_SCALABILITY_LINUX
#include <stdio.h>
static FILE* scalability_msg_count;
#endif

// foo

DDS_Topic g_DCPSParticipant_topic;
Sample_t dcps_participant_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSParticipant dcps_participant_sample_data[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSParticipant dcps_participant_incomingSample_data;
DDS_DataReader g_DCPSParticipant_reader;
DDS_DataWriter g_DCPSParticipant_writer;

DDS_Topic g_DCPSTopic_topic;
Sample_t dcps_topic_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static DDS_DCPSTopic dcps_topic_sample_data[SDDS_QOS_HISTORY_DEPTH];
static DDS_DCPSTopic dcps_topic_incomingSample_data;
DDS_DataReader g_DCPSTopic_reader;
DDS_DataWriter g_DCPSTopic_writer;

DDS_Topic g_DCPSPublication_topic;
Sample_t dcps_publication_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static DDS_DCPSPublication dcps_publication_sample_data[SDDS_QOS_HISTORY_DEPTH];
static DDS_DCPSPublication dcps_publication_incomingSample_data;
DDS_DataReader g_DCPSPublication_reader;
DDS_DataWriter g_DCPSPublication_writer;

DDS_Topic g_DCPSSubscription_topic;
Sample_t dcps_subscription_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSSubscription dcps_subscription_sample_data[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSSubscription dcps_subscription_incomingSample_data;
DDS_DataReader g_DCPSSubscription_reader;
DDS_DataWriter g_DCPSSubscription_writer;

#ifdef FEATURE_SDDS_SECURITY_ENABLED
DDS_Topic g_ParticipantStatelessMessage_topic;
Sample_t dcps_psm_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static DDS_ParticipantStatelessMessage dcps_psm_sample_data[SDDS_QOS_HISTORY_DEPTH];
static DDS_ParticipantStatelessMessage dcps_psm_incomingSample_data;
DDS_DataReader g_ParticipantStatelessMessage_reader;
DDS_DataWriter g_ParticipantStatelessMessage_writer;
#endif

#ifdef FEATURE_SDDS_LOCATION_ENABLED
DDS_Topic g_DCPSLocation_topic;
Sample_t dcps_location_samples_pool[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSLocation dcps_location_sample_data[SDDS_QOS_HISTORY_DEPTH];
static SDDS_DCPSLocation dcps_location_incomingSample_data;
DDS_DataReader g_DCPSLocation_reader;
DDS_DataWriter g_DCPSLocation_writer;
#endif

Topic_t*
sDDS_DCPSParticipantTopic_create();
Topic_t*
sDDS_DCPSTopicTopic_create();
Topic_t*
sDDS_DCPSPublicationTopic_create();
Topic_t*
sDDS_DCPSSubscriptionTopic_create();
Topic_t*
sDDS_ParticipantStatelessMessageTopic_create();
Topic_t*
sDDS_DCPSLocationTopic_create();

static uint8_t generalByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t participantByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t subPubByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
static uint8_t topicByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];

#ifdef FEATURE_SDDS_LOCATION_ENABLED
static uint8_t locationByteAddr[SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE];
#endif

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
#ifdef FEATURE_SDDS_SECURITY_ENABLED
        dcps_psm_samples_pool[index].data = &dcps_psm_sample_data[index];
#endif
#ifdef FEATURE_SDDS_LOCATION_ENABLED
        dcps_location_samples_pool[index].data = &dcps_location_sample_data[index];
#endif
    }

    BuiltinTopic_participantID = NodeConfig_getNodeID();

    /* Participant */

    g_DCPSParticipant_topic = sDDS_DCPSParticipantTopic_create();
    if (g_DCPSParticipant_topic == NULL){
        return SDDS_RT_FAIL;
    }
    g_DCPSParticipant_reader = DataSink_create_datareader(g_DCPSParticipant_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSParticipant_reader), dcps_participant_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSParticipant_writer = DataSource_create_datawriter(g_DCPSParticipant_topic, NULL, NULL, NULL);

    ret = LocatorDB_findLocatorByMcastAddr(SDDS_BUILTIN_PARTICIPANT_ADDRESS, &l);
    if (ret == SDDS_RT_OK) {
        Locator_upRef(l);
    }
    else {
        ret = LocatorDB_newMultiLocator(&l);
        if (ret != SDDS_RT_OK) {
            return ret;
        }

        ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_PARTICIPANT_ADDRESS);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(l);
            return ret;
        }
    }

    ret = Topic_addRemoteDataSink(g_DCPSParticipant_topic, l, SDDS_TOPIC_SUBSCRIPTION_MCAST_PARTICIPANT, ACTIVE);
    if (ret != SDDS_RT_OK) {
        Locator_downRef(l);
        return ret;
    }

    /* Topic */

    g_DCPSTopic_topic = sDDS_DCPSTopicTopic_create();
    if(g_DCPSTopic_topic == NULL){
        return SDDS_RT_FAIL;
    }
    g_DCPSTopic_reader = DataSink_create_datareader(g_DCPSTopic_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSTopic_reader), dcps_topic_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSTopic_writer = DataSource_create_datawriter(g_DCPSTopic_topic, NULL, NULL, NULL);

    ret = LocatorDB_findLocatorByMcastAddr(SDDS_BUILTIN_TOPIC_ADDRESS, &l);
    if (ret == SDDS_RT_OK) {
        Locator_upRef(l);
    }
    else {
        ret = LocatorDB_newMultiLocator(&l);
        if (ret != SDDS_RT_OK) {
            return ret;
        }

        ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_TOPIC_ADDRESS);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(l);
            return ret;
        }
    }

    ret = Topic_addRemoteDataSink(g_DCPSTopic_topic, l, SDDS_TOPIC_SUBSCRIPTION_MCAST_PARTICIPANT, ACTIVE);
    if (ret != SDDS_RT_OK) {
        Locator_downRef(l);
        return ret;
    }

    /* Publication */

    g_DCPSPublication_topic = sDDS_DCPSPublicationTopic_create();
    if(g_DCPSPublication_topic == NULL){
        return SDDS_RT_FAIL;
    }
    g_DCPSPublication_reader = DataSink_create_datareader(g_DCPSPublication_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSPublication_reader), dcps_publication_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSPublication_writer = DataSource_create_datawriter(g_DCPSPublication_topic, NULL, NULL, NULL);

    ret = LocatorDB_findLocatorByMcastAddr(SDDS_BUILTIN_SUB_PUB_ADDRESS, &l);
    if (ret == SDDS_RT_OK) {
        Locator_upRef(l);
    }
    else {
        ret = LocatorDB_newMultiLocator(&l);
        if (ret != SDDS_RT_OK) {
            return ret;
        }

        ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_SUB_PUB_ADDRESS);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(l);
            return ret;
        }
    }

    ret = Topic_addRemoteDataSink(g_DCPSPublication_topic, l, SDDS_TOPIC_SUBSCRIPTION_MCAST_PARTICIPANT, ACTIVE);
    if (ret != SDDS_RT_OK) {
        Locator_downRef(l);
        return ret;
    }

    /* Subscription */

    g_DCPSSubscription_topic = sDDS_DCPSSubscriptionTopic_create();
    if(g_DCPSSubscription_topic ==  NULL){
        return SDDS_RT_FAIL;
    }
    g_DCPSSubscription_reader = DataSink_create_datareader(g_DCPSSubscription_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSSubscription_reader), dcps_subscription_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSSubscription_writer = DataSource_create_datawriter(g_DCPSSubscription_topic, NULL, NULL, NULL);

    ret = LocatorDB_findLocatorByMcastAddr(SDDS_BUILTIN_SUB_PUB_ADDRESS, &l);
    if (ret == SDDS_RT_OK) {
        Locator_upRef(l);
    }
    else {
        ret = LocatorDB_newMultiLocator(&l);
        if (ret != SDDS_RT_OK) {
            return ret;
        }

        ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_SUB_PUB_ADDRESS);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(l);
            return ret;
        }
    }

    ret = Topic_addRemoteDataSink(g_DCPSSubscription_topic, l, SDDS_TOPIC_SUBSCRIPTION_MCAST_PARTICIPANT, ACTIVE);
    if (ret != SDDS_RT_OK) {
        Locator_downRef(l);
        return ret;
    }

    /* Security */
#ifdef FEATURE_SDDS_SECURITY_ENABLED
    g_ParticipantStatelessMessage_topic = sDDS_ParticipantStatelessMessageTopic_create();
    if (g_ParticipantStatelessMessage_topic == NULL){
        return SDDS_RT_FAIL;
    }
    g_ParticipantStatelessMessage_reader = DataSink_create_datareader(g_ParticipantStatelessMessage_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_ParticipantStatelessMessage_reader), dcps_psm_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_ParticipantStatelessMessage_writer = DataSource_create_datawriter(g_ParticipantStatelessMessage_topic, NULL, NULL, NULL);

    ret = LocatorDB_findLocatorByMcastAddr(SDDS_BUILTIN_PAR_STATE_MSG_ADDRESS, &l);
    if (ret == SDDS_RT_OK) {
        Locator_upRef(l);
    }
    else {
        ret = LocatorDB_newMultiLocator(&l);
        if (ret != SDDS_RT_OK) {
            return ret;
        }

        ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_PAR_STATE_MSG_ADDRESS);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(l);
            return ret;
        }
    }

    ret = Topic_addRemoteDataSink(g_ParticipantStatelessMessage_topic, l, SDDS_TOPIC_SUBSCRIPTION_MCAST_PARTICIPANT, ACTIVE);
    if (ret != SDDS_RT_OK) {
        Locator_downRef(l);
        return ret;
    }
#endif
    /* Location Built-In Topic */
#ifdef FEATURE_SDDS_LOCATION_ENABLED
    g_DCPSLocation_topic = sDDS_DCPSLocationTopic_create();
    if (g_DCPSLocation_topic == NULL){
        return SDDS_RT_FAIL;
    }
    g_DCPSLocation_reader = DataSink_create_datareader(g_DCPSLocation_topic, NULL, NULL, NULL);
    sdds_History_setup (DataReader_history (g_DCPSLocation_reader), dcps_location_samples_pool, SDDS_QOS_HISTORY_DEPTH);
    g_DCPSLocation_writer = DataSource_create_datawriter(g_DCPSLocation_topic, NULL, NULL, NULL);

    ret = LocatorDB_findLocatorByMcastAddr(SDDS_BUILTIN_LOCATION_ADDRESS, &l);
    if (ret == SDDS_RT_OK) {
        Locator_upRef(l);
    }
    else {
        ret = LocatorDB_newMultiLocator(&l);
        if (ret != SDDS_RT_OK) {
            return ret;
        }

        ret = Network_setMulticastAddressToLocator(l, SDDS_BUILTIN_LOCATION_ADDRESS);
        if (ret != SDDS_RT_OK) {
            Locator_downRef(l);
            return ret;
        }
    }

    ret = Topic_addRemoteDataSink(g_DCPSLocation_topic, l, SDDS_TOPIC_SUBSCRIPTION_MCAST_PARTICIPANT, ACTIVE);
    if (ret != SDDS_RT_OK) {
        Locator_downRef(l);
        return ret;
    }
#endif

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
#   ifdef FEATURE_SDDS_LOCATION_ENABLED
    ret = SNPS_IPv6_str2Addr(SDDS_BUILTIN_LOCATION_ADDRESS, locationByteAddr, &addrLen);
    if (ret != SDDS_RT_OK) {
        return ret;
    }
#   endif
#else
    // TO DO
#endif
    return SDDS_RT_OK;
}

/********************
* DCPS Participant *
********************/

void*
TopicMarshalling_DCPSParticipant_getPrimaryKey(Data data);

void*
TopicMarshalling_DCPSParticipant_getSecondaryKey(Data data);

rc_t
TopicMarshalling_DCPSParticipant_cmpPrimaryKeys(Data data1, Data data2);

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

#ifdef DISCOVERY_UNICAST_SUBSCRIPTION
    castType = SDDS_SNPS_CAST_UNICAST;
    addr = "";
    addrLen = 0;
#endif

    rc_t ret = DataWriter_writeAddress((DataWriter_t*) _this, castType, addrType, addr, addrLen);
    ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);
    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
#ifdef TEST_SCALABILITY_LINUX
    	scalability_msg_count = fopen(SCALABILITY_LOG, "a");
    	fwrite("I", 1, 1, scalability_msg_count);
		fclose(scalability_msg_count);
#endif

#ifdef TEST_SCALABILITY_RIOT
        fprintf(stderr,"{SCL:I}\n");
#endif
        return DDS_RETCODE_OK;
    }
    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSParticipantTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_participant_incomingSample_data;
    topic->Data_encode = TopicMarshalling_DCPSParticipant_encode;
    topic->Data_decode = TopicMarshalling_DCPSParticipant_decode;

    topic->domain = DDS_DCPS_PARTICIPANT_DOMAIN;
    topic->id = DDS_DCPS_PARTICIPANT_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSParticipant_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->Data_cmpPrimaryKeys = TopicMarshalling_DCPSParticipant_cmpPrimaryKeys;
    topic->Data_getPrimaryKey = TopicMarshalling_DCPSParticipant_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_DCPSParticipant_getSecondaryKey;

    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }

    return topic;
}

void*
TopicMarshalling_DCPSParticipant_getPrimaryKey(Data data) {
    DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_DCPSParticipant_getSecondaryKey(Data data) {
    DDS_DCPSParticipant* real_data = (DDS_DCPSParticipant*) data;
    return (void*) &real_data->skey;
}


rc_t
TopicMarshalling_DCPSParticipant_cmpPrimaryKeys(Data data1, Data data2) {
    DDS_DCPSParticipant* real_data1 = (DDS_DCPSParticipant*) data1;
    DDS_DCPSParticipant* real_data2 = (DDS_DCPSParticipant*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
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
        Log_debug("size mismatch is %zu should be %zu \n", *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    SNPS_Address_t address;
    rc_t ret = DataSink_getAddr(&address);

    SDDS_DCPSParticipant* sdds_data = (SDDS_DCPSParticipant*) data;

    sdds_data->addr = address.addr;
    sdds_data->srcAddr = buffer->locators->first_fn(buffer->locators);
    assert(sdds_data->srcAddr);
    Locator_upRef(sdds_data->srcAddr);

    return SDDS_RT_OK;
}

/***************
*  DCPS_TOPIC *
***************/

void*
TopicMarshalling_DCPSTopic_getPrimaryKey(Data data);

void*
TopicMarshalling_DCPSTopic_getSecondaryKey(Data data);

rc_t
TopicMarshalling_DCPSTopic_cmpPrimaryKeys(Data data1, Data data2);

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
#ifdef TEST_SCALABILITY_LINUX
    	scalability_msg_count = fopen(SCALABILITY_LOG, "a");
    	fwrite("T", 1, 1, scalability_msg_count);
		fclose(scalability_msg_count);
#endif

#ifdef TEST_SCALABILITY_RIOT
        fprintf(stderr,"{SCL:T}\n");
#endif
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSTopicTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_topic_incomingSample_data;

    topic->Data_encode = TopicMarshalling_DCPSTopic_encode;
    topic->Data_decode = TopicMarshalling_DCPSTopic_decode;

    topic->domain = DDS_DCPS_TOPIC_DOMAIN;
    topic->id = DDS_DCPS_TOPIC_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSTopic_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->Data_cmpPrimaryKeys = TopicMarshalling_DCPSTopic_cmpPrimaryKeys;
    topic->Data_getPrimaryKey = TopicMarshalling_DCPSTopic_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_DCPSTopic_getSecondaryKey;

    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }

    return topic;
}

void*
TopicMarshalling_DCPSTopic_getPrimaryKey(Data data) {
    DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_DCPSTopic_getSecondaryKey(Data data) {
    DDS_DCPSTopic* real_data = (DDS_DCPSTopic*) data;
    return (void*) &real_data->skey;
}

rc_t
TopicMarshalling_DCPSTopic_cmpPrimaryKeys(Data data1, Data data2) {
    DDS_DCPSTopic* real_data1 = (DDS_DCPSTopic*) data1;
    DDS_DCPSTopic* real_data2 = (DDS_DCPSTopic*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
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
        Log_debug("size mismatch is %zu should be %zu \n", *size, expectedSize);
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

void*
TopicMarshalling_DCPSPublication_getPrimaryKey(Data data);

void*
TopicMarshalling_DCPSPublication_getSecondaryKey(Data data);

rc_t
TopicMarshalling_DCPSPublication_cmpPrimaryKeys(Data data1, Data data2);

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
#ifdef TEST_SCALABILITY_LINUX
    	scalability_msg_count = fopen(SCALABILITY_LOG, "a");
    	fwrite("P", 1, 1, scalability_msg_count);
		fclose(scalability_msg_count);
#endif

#ifdef TEST_SCALABILITY_RIOT
        fprintf(stderr, "{SCL:P}\n");
#endif
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSPublicationTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_publication_incomingSample_data;

    topic->Data_encode = TopicMarshalling_DCPSPublication_encode;
    //topic->dsinks.list = locator;

    topic->Data_decode = TopicMarshalling_DCPSPublication_decode;

    topic->domain = DDS_DCPS_PUBLICATION_DOMAIN;
    topic->id = DDS_DCPS_PUBLICATION_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSPublication_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->Data_cmpPrimaryKeys = TopicMarshalling_DCPSPublication_cmpPrimaryKeys;
    topic->Data_getPrimaryKey = TopicMarshalling_DCPSPublication_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_DCPSPublication_getSecondaryKey;

    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }

    return topic;
}

void*
TopicMarshalling_DCPSPublication_getPrimaryKey(Data data) {
    DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_DCPSPublication_getSecondaryKey(Data data) {
    DDS_DCPSPublication* real_data = (DDS_DCPSPublication*) data;
    return (void*) &real_data->skey;
}

rc_t
TopicMarshalling_DCPSPublication_cmpPrimaryKeys(Data data1, Data data2) {
    DDS_DCPSPublication* real_data1 = (DDS_DCPSPublication*) data1;
    DDS_DCPSPublication* real_data2 = (DDS_DCPSPublication*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
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
        Log_debug("size mismatch is %zu should be %zu \n", *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_dec_uint16(start + *size, &real_data->participant_key);
    *size += sizeof(real_data->participant_key);

    Marshalling_dec_uint16(start + *size, &real_data->topic_id);
    *size += sizeof(real_data->topic_id);

    real_data->srcAddr = buffer->locators->first_fn(buffer->locators);
    assert(real_data->srcAddr);
    Locator_upRef(real_data->srcAddr);

    return SDDS_RT_OK;
}

/*********************
* DCPS Subscription *
*********************/

void*
TopicMarshalling_DCPSSubscription_getPrimaryKey(Data data);

void*
TopicMarshalling_DCPSSubscription_getSecondaryKey(Data data);

rc_t
TopicMarshalling_DCPSSubscription_cmpPrimaryKeys(Data data1, Data data2);

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

#ifdef DISCOVERY_UNICAST_SUBSCRIPTION
    castType = SDDS_SNPS_CAST_UNICAST;
    addr = "";
    addrLen = 0;
#endif

    rc_t ret = DataWriter_writeAddress((DataWriter_t*) _this, castType, addrType, addr, addrLen);
    ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);

    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
#ifdef TEST_SCALABILITY_LINUX
    	scalability_msg_count = fopen(SCALABILITY_LOG, "a");
    	fwrite("S", 1, 1, scalability_msg_count);
		fclose(scalability_msg_count);
#endif

#ifdef TEST_SCALABILITY_RIOT
        fprintf(stderr,"{SCL:S}\n");
#endif
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSSubscriptionTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_subscription_incomingSample_data;

    topic->Data_encode = TopicMarshalling_DCPSSubscription_encode;

    topic->Data_decode = TopicMarshalling_DCPSSubscription_decode;

    topic->domain = DDS_DCPS_SUBSCRIPTION_DOMAIN;
    topic->id = DDS_DCPS_SUBSCRIPTION_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSSubscription_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->Data_cmpPrimaryKeys = TopicMarshalling_DCPSSubscription_cmpPrimaryKeys;
    topic->Data_getPrimaryKey = TopicMarshalling_DCPSSubscription_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_DCPSSubscription_getSecondaryKey;

    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }
    return topic;
}

void*
TopicMarshalling_DCPSSubscription_getPrimaryKey(Data data) {
    DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_DCPSSubscription_getSecondaryKey(Data data) {
    DDS_DCPSSubscription* real_data = (DDS_DCPSSubscription*) data;
    return (void*) &real_data->skey;
}

rc_t
TopicMarshalling_DCPSSubscription_cmpPrimaryKeys(Data data1, Data data2) {
    DDS_DCPSSubscription* real_data1 = (DDS_DCPSSubscription*) data1;
    DDS_DCPSSubscription* real_data2 = (DDS_DCPSSubscription*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
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
        Log_debug("size mismatch is %zu should be %zu \n", *size, expectedSize);
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

    return SDDS_RT_OK;
}

/******************************
* ParticipantStatelessMessage *
*******************************/
/* Security */

#ifdef FEATURE_SDDS_SECURITY_ENABLED

void*
TopicMarshalling_ParticipantStatelessMessage_getPrimaryKey(Data data);

void*
TopicMarshalling_ParticipantStatelessMessage_getSecondaryKey(Data data);

rc_t
TopicMarshalling_ParticipantStatelessMessage_cmpPrimaryKeys(Data data1, Data data2);

rc_t
TopicMarshalling_ParticipantStatelessMessage_cpy(Data dest, Data source);

rc_t
TopicMarshalling_ParticipantStatelessMessage_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_ParticipantStatelessMessageDataReader_take_next_sample(
                                         DDS_DataReader _this,
                                         DDS_ParticipantStatelessMessage** data_values,
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
TopicMarshalling_ParticipantStatelessMessage_encode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_ParticipantStatelessMessageDataWriter_write(
                              DDS_DataWriter _this,
                              const DDS_ParticipantStatelessMessage* instance_data,
                              const DDS_InstanceHandle_t handle
                              ) {
    rc_t ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);

    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
        return DDS_RETCODE_OK;
    }

    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_ParticipantStatelessMessageTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_psm_incomingSample_data;

    topic->Data_encode = TopicMarshalling_ParticipantStatelessMessage_encode;
    topic->Data_decode = TopicMarshalling_ParticipantStatelessMessage_decode;
    topic->domain = DDS_PARTICIPANT_STATELESS_MESSAGE_DOMAIN;
    topic->id = DDS_PARTICIPANT_STATELESS_MESSAGE_TOPIC;
    topic->Data_cpy = TopicMarshalling_ParticipantStatelessMessage_cpy;
    topic->Data_cmpPrimaryKeys = TopicMarshalling_ParticipantStatelessMessage_cmpPrimaryKeys;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->Data_getPrimaryKey = TopicMarshalling_ParticipantStatelessMessage_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_ParticipantStatelessMessage_getSecondaryKey;

    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }

    return topic;
}

void*
TopicMarshalling_ParticipantStatelessMessage_getPrimaryKey(Data data) {
    DDS_ParticipantStatelessMessage* real_data = (DDS_ParticipantStatelessMessage*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_ParticipantStatelessMessage_getSecondaryKey(Data data) {
    DDS_ParticipantStatelessMessage* real_data = (DDS_ParticipantStatelessMessage*) data;
    return (void*) &real_data->skey;
}

rc_t
TopicMarshalling_ParticipantStatelessMessage_cmpPrimaryKeys(Data data1, Data data2) {
    DDS_ParticipantStatelessMessage* real_data1 = (DDS_ParticipantStatelessMessage*) data1;
    DDS_ParticipantStatelessMessage* real_data2 = (DDS_ParticipantStatelessMessage*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
}

rc_t
TopicMarshalling_ParticipantStatelessMessage_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(DDS_ParticipantStatelessMessage));

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_ParticipantStatelessMessage_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;
    byte_t* start = buffer->buff_start + buffer->curPos + 1;
    DDS_ParticipantStatelessMessage* real_data = (DDS_ParticipantStatelessMessage*) data;

    Marshalling_enc_uint16(start + *size, &real_data->msgid);
    *size += sizeof(real_data->msgid);

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_enc_string(start + *size, real_data->message_class_id, CLASS_ID_STRLEN);
    *size += CLASS_ID_STRLEN;

    Marshalling_enc_string(start + *size, real_data->message_data.class_id, CLASS_ID_STRLEN);
    *size += CLASS_ID_STRLEN;

    for(int i = 0; i < PROPERTIES_NUM; i++) {
        Marshalling_enc_string(start + *size, real_data->message_data.props[i].key, PROPERTY_KEY_STRLEN);
        *size += PROPERTY_KEY_STRLEN;
        Marshalling_enc_string(start + *size, real_data->message_data.props[i].value, PROPERTY_VAL_STRLEN);
        *size += PROPERTY_VAL_STRLEN;
    }

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_ParticipantStatelessMessage_decode(NetBuffRef_t* buffer, Data data, size_t* size) {

    *size = 0;
    byte_t* start = buffer->buff_start + buffer->curPos;
    DDS_ParticipantStatelessMessage* real_data = (DDS_ParticipantStatelessMessage*) data;

    Marshalling_dec_uint16(start + *size, &real_data->msgid);
    *size += sizeof(real_data->msgid);

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);

    Marshalling_dec_string(start + *size, real_data->message_class_id, CLASS_ID_STRLEN);
    *size += CLASS_ID_STRLEN;

    Marshalling_dec_string(start + *size, real_data->message_data.class_id, CLASS_ID_STRLEN);
    *size += CLASS_ID_STRLEN;

    for(int i = 0; i < PROPERTIES_NUM; i++) {
        Marshalling_dec_string(start + *size, real_data->message_data.props[i].key, PROPERTY_KEY_STRLEN);
        *size += PROPERTY_KEY_STRLEN;
        Marshalling_dec_string(start + *size, real_data->message_data.props[i].value, PROPERTY_VAL_STRLEN);
        *size += PROPERTY_VAL_STRLEN;
    }

    return SDDS_RT_OK;

}
#endif

/********************
* DCPS Location *
********************/

#ifdef FEATURE_SDDS_LOCATION_ENABLED

void*
TopicMarshalling_DCPSLocation_getPrimaryKey(Data data);

void*
TopicMarshalling_DCPSLocation_getSecondaryKey(Data data);


rc_t
TopicMarshalling_DCPSLocation_cmpPrimaryKeys(Data data1, Data data2);

rc_t
TopicMarshalling_DCPSLocation_cpy(Data dest, Data source);

rc_t
TopicMarshalling_DCPSLocation_decode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSLocationDataReader_take_next_sample(
                                               DDS_DataReader _this,
                                               SDDS_DCPSLocation** data_values,
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
TopicMarshalling_DCPSLocation_encode(NetBuffRef_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t
DDS_DCPSLocationDataWriter_write(
                                    DDS_DataWriter _this,
                                    const SDDS_DCPSLocation* instance_data,
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

#ifdef DISCOVERY_UNICAST_SUBSCRIPTION
    castType = SDDS_SNPS_CAST_UNICAST;
    addr = "";
    addrLen = 0;
#endif

    rc_t ret = DataWriter_writeAddress((DataWriter_t*) _this, castType, addrType, addr, addrLen);
    ret = DataWriter_write((DataWriter_t*) _this, (Data)instance_data, (void*) handle);
    if ((ret == SDDS_RT_OK) || (ret == SDDS_RT_DEFERRED)) {
        return DDS_RETCODE_OK;
    }
    return DDS_RETCODE_ERROR;
}

Topic_t*
sDDS_DCPSLocationTopic_create() {
    Topic_t* topic = TopicDB_createTopic();

    topic->incomingSample.data = &dcps_location_incomingSample_data;
    topic->Data_encode = TopicMarshalling_DCPSLocation_encode;
    topic->Data_decode = TopicMarshalling_DCPSLocation_decode;

    topic->domain = SDDS_DCPS_LOCATION_DOMAIN;
    topic->id = SDDS_DCPS_LOCATION_TOPIC;
    topic->Data_cpy = TopicMarshalling_DCPSLocation_cpy;
    topic->dsinks.list = List_initConcurrentLinkedList();
    topic->Data_cmpPrimaryKeys = TopicMarshalling_DCPSLocation_cmpPrimaryKeys;
    topic->Data_getPrimaryKey = TopicMarshalling_DCPSLocation_getPrimaryKey;
    topic->Data_getSecondaryKey = TopicMarshalling_DCPSLocation_getSecondaryKey;

    if(topic->dsinks.list == NULL){
        return NULL;
    }
    topic->dsources.list = List_initConcurrentLinkedList();
    if(topic->dsources.list == NULL){
        return NULL;
    }

    return topic;
}

void*
TopicMarshalling_DCPSLocation_getPrimaryKey(Data data) {
    SDDS_DCPSLocation* real_data = (SDDS_DCPSLocation*) data;
    return (void*) &real_data->pkey;
}

void*
TopicMarshalling_DCPSLocation_getSecondaryKey(Data data) {
    SDDS_DCPSLocation* real_data = (SDDS_DCPSLocation*) data;
    return (void*) &real_data->skey;
}

rc_t
TopicMarshalling_DCPSLocation_cmpPrimaryKeys(Data data1, Data data2) {
    SDDS_DCPSLocation* real_data1 = (SDDS_DCPSLocation*) data1;
    SDDS_DCPSLocation* real_data2 = (SDDS_DCPSLocation*) data2;

    if (real_data1->pkey == real_data2->pkey) {
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
}

rc_t
TopicMarshalling_DCPSLocation_cpy(Data dest, Data source) {
    memcpy(dest, source, sizeof(SDDS_DCPSLocation));

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSLocation_encode(NetBuffRef_t* buffer, Data data, size_t* size) {
    *size = 0;

    SDDS_DCPSLocation* real_data = (SDDS_DCPSLocation*) data;

    int maxSize = 0;
    maxSize += sizeof(real_data->key);
    maxSize += sizeof(real_data->device);
    maxSize += sizeof(real_data->x);
    maxSize += sizeof(real_data->y);
    maxSize += sizeof(real_data->z);
    maxSize += sizeof(real_data->width);
    maxSize += sizeof(real_data->length);
    maxSize += sizeof(real_data->expiration);
    maxSize += sizeof(real_data->age);

    byte_t* start = buffer->buff_start + buffer->curPos + 1;

    if ((buffer->curPos + maxSize + 1) >= SDDS_NET_MAX_BUF_SIZE) {
        return SDDS_RT_FAIL;
    }

    Marshalling_enc_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);
    Marshalling_enc_uint16(start + *size, &real_data->device);
    *size += sizeof(real_data->device);
    Marshalling_enc_uint16(start + *size, &real_data->x);
    *size += sizeof(real_data->x);
    Marshalling_enc_uint16(start + *size, &real_data->y);
    *size += sizeof(real_data->y);
    Marshalling_enc_uint16(start + *size, &real_data->z);
    *size += sizeof(real_data->z);
    Marshalling_enc_uint16(start + *size, &real_data->width);
    *size += sizeof(real_data->width);
    Marshalling_enc_uint16(start + *size, &real_data->length);
    *size += sizeof(real_data->length);
    Marshalling_enc_int16(start + *size, &real_data->expiration);
    *size += sizeof(real_data->expiration);
    Marshalling_enc_int16(start + *size, &real_data->age);
    *size += sizeof(real_data->age);

    return SDDS_RT_OK;
}

rc_t
TopicMarshalling_DCPSLocation_decode(NetBuffRef_t* buffer, Data data, size_t* size) {
    SDDS_DCPSLocation* real_data = (SDDS_DCPSLocation*) data;

    size_t expectedSize = 0;
    expectedSize += sizeof(real_data->key);
    expectedSize += sizeof(real_data->device);
    expectedSize += sizeof(real_data->x);
    expectedSize += sizeof(real_data->y);
    expectedSize += sizeof(real_data->z);
    expectedSize += sizeof(real_data->width);
    expectedSize += sizeof(real_data->length);
    expectedSize += sizeof(real_data->expiration);
    expectedSize += sizeof(real_data->age);
    if (*size != expectedSize) {
        Log_debug("size mismatch is %zu should be %zu \n", *size, expectedSize);
    }

    byte_t* start = buffer->buff_start + buffer->curPos;

    *size = 0;

    Marshalling_dec_uint16(start + *size, &real_data->key);
    *size += sizeof(real_data->key);
    Marshalling_dec_uint16(start + *size, &real_data->device);
    *size += sizeof(real_data->device);
    Marshalling_dec_uint16(start + *size, &real_data->x);
    *size += sizeof(real_data->x);
    Marshalling_dec_uint16(start + *size, &real_data->y);
    *size += sizeof(real_data->y);
    Marshalling_dec_uint16(start + *size, &real_data->z);
    *size += sizeof(real_data->z);
    Marshalling_dec_uint16(start + *size, &real_data->width);
    *size += sizeof(real_data->width);
    Marshalling_dec_uint16(start + *size, &real_data->length);
    *size += sizeof(real_data->length);
    Marshalling_dec_int16(start + *size, &real_data->expiration);
    *size += sizeof(real_data->expiration);
    Marshalling_dec_int16(start + *size, &real_data->age);
    *size += sizeof(real_data->age);

    return SDDS_RT_OK;
}

#endif

bool
BuildinTopic_isBuiltinTopic(topicid_t tID, domainid_t dID) {
    if (
        (DDS_DCPS_PARTICIPANT_DOMAIN == dID) && (DDS_DCPS_PARTICIPANT_TOPIC == tID)
       || (DDS_DCPS_TOPIC_DOMAIN == dID) && (DDS_DCPS_TOPIC_TOPIC == tID)
       || (DDS_DCPS_PUBLICATION_DOMAIN == dID) && (DDS_DCPS_PUBLICATION_TOPIC == tID)
       || (DDS_DCPS_SUBSCRIPTION_DOMAIN == dID) && (DDS_DCPS_SUBSCRIPTION_TOPIC == tID)
       || (DDS_PARTICIPANT_STATELESS_MESSAGE_DOMAIN == dID) && (DDS_PARTICIPANT_STATELESS_MESSAGE_TOPIC == tID)
#   ifdef FEATURE_SDDS_LOCATION_ENABLED
       || (SDDS_DCPS_LOCATION_DOMAIN == dID) && (SDDS_DCPS_LOCATION_TOPIC == tID)
#   endif
#   ifdef FEATURE_SDDS_MANAGEMENT_TOPIC_ENABLED
       || (SDDS_DCPS_MANAGEMENT_DOMAIN == dID) && (SDDS_DCPS_MANAGEMENT_TOPIC == tID)
#   endif
        ) {
        return true;
    }

    return false;
}

#endif
