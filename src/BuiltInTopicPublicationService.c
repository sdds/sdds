/*
 * BuiltInTopicPublicationService.c
 *
 *  Created on: Aug 1, 2016
 *      Author: o_dedi
 */

#include "BuiltInTopicPublicationService.h"

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED

static Task sendPartTask;
static Task sendTopTask;
static Task sendPubTask;
static Task sendLocTask;

#   ifndef BUILTIN_TOPIC_PUBLICATION_MAX_PUBS
#define BUILTIN_TOPIC_PUBLICATION_MAX_PUBS 20
#   endif

static void
s_BuiltInTopicPublicationService_publishDCPSParticipant();
static void
s_BuiltInTopicPublicationService_publishDCPSTopic();
static void
s_BuiltInTopicPublicationService_publishDCPSPublication();
static void
s_BuiltInTopicPublicationService_publishDCPSLocation();


rc_t
BuiltInTopicPublicationService_init() {
#   if (SDDS_BUILTIN_TOPIC_PARTICIPANT_TIMER != 0)
    sendPartTask = Task_create();
    Task_init(sendPartTask, s_BuiltInTopicPublicationService_publishDCPSParticipant, NULL);
    if (Task_start(sendPartTask, SDDS_BUILTIN_TOPIC_PARTICIPANT_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
    }
#   endif

#   if (SDDS_BUILTIN_TOPIC_TOPIC_TIMER != 0)
    sendTopTask = Task_create();
    Task_init(sendTopTask, s_BuiltInTopicPublicationService_publishDCPSTopic, NULL);
    if (Task_start(sendTopTask, SDDS_BUILTIN_TOPIC_TOPIC_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
    }
#   endif

#   if defined(SDDS_HAS_SUB) && (SDDS_BUILTIN_TOPIC_PUBLICATION_TIMER != 0)
    sendPubTask = Task_create();
    Task_init(sendPubTask, s_BuiltInTopicPublicationService_publishDCPSPublication, NULL);
    if (Task_start(sendPubTask, SDDS_BUILTIN_TOPIC_PUBLICATION_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
    }
#   endif


#   if defined(FEATURE_SDDS_LOCATION_TRACKING_ENABLED) && (SDDS_BUILTIN_TOPIC_LOCATION_TIMER != 0)
    sendLocTask = Task_create();
    Task_init(sendLocTask, s_BuiltInTopicPublicationService_publishDCPSLocation, NULL);
    if (Task_start(sendLocTask, SDDS_BUILTIN_TOPIC_LOCATION_TIMER, 0, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
    }
#   endif

    BuiltInTopicPublicationService_publishDCPSParticipant();
    BuiltInTopicPublicationService_publishDCPSPublication();

    return SDDS_RT_OK;
}

rc_t
BuiltInTopicPublicationService_publishDCPSParticipant() {
    SDDS_DCPSParticipant p;
    DataWriter_t* dw = ((DataWriter_t*) g_DCPSParticipant_writer);
    p.data.key = BuiltinTopic_participantID; //(BuiltinTopic_participantID << 4) | dw->id;
    p.participantID = BuiltinTopic_participantID;

    if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer, (DDS_DCPSParticipant *)&p,
                                            NULL) == DDS_RETCODE_ERROR) {
        // handle error
        Log_error("Send participant topic failed.\n");
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

static void
s_BuiltInTopicPublicationService_publishDCPSParticipant() {
    if (BuiltInTopicPublicationService_publishDCPSParticipant() != SDDS_RT_OK) {
        Log_error("Failed to publish DCPSParticipant.\n");
    }
}

rc_t
BuiltInTopicPublicationService_publishDCPSTopic(topicid_t id) {
    // To Do
}

static void
s_BuiltInTopicPublicationService_publishDCPSTopic() {
    // To Do
}

rc_t
BuiltInTopicPublicationService_publishDCPSPublication() {
#   if defined(SDDS_TOPIC_HAS_SUB)
    rc_t ret;
    DDS_DCPSPublication pubT[BUILTIN_TOPIC_PUBLICATION_MAX_PUBS];
    int len = 0;

    ret = DataSource_getDataWrites(pubT, &len);

    if (ret == SDDS_RT_OK) {
        int i;
        for (i = 0; i < len; i++) {
            Log_debug("Send (publication):[%u][%x] topic:%u\n",
                      pubT[i].key, pubT[i].participant_key,
                      pubT[i].topic_id);
            DDS_ReturnCode_t r = DDS_DCPSPublicationDataWriter_write(g_DCPSPublication_writer,
                                                    &pubT[i],
                                                    NULL);
            if (r == DDS_RETCODE_ERROR) {
                // handle error
                Log_error("Send publication topic failed.\n");
                return SDDS_RT_FAIL;
            }
        }
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
#   endif
}

static void
s_BuiltInTopicPublicationService_publishDCPSPublication() {
    if (BuiltInTopicPublicationService_publishDCPSPublication() != SDDS_RT_OK) {
        Log_error("Failed to publish DCPSPublication.\n");
    }
}

rc_t
BuiltInTopicPublicationService_publishDCPSSubscription(topicid_t id) {
    SDDS_DCPSSubscription st_data_used;
    DDS_ReturnCode_t ret;

    ret = DataSink_getTopic((DDS_DCPSSubscription*) &st_data_used, id, NULL);
    if (ret == SDDS_RT_OK) {
        Log_debug("Send (subscription):[%u][%x] topic:%u\n",
                  st_data_used.data.key, st_data_used.data.participant_key,
                  st_data_used.data.topic_id);
        if (DDS_DCPSSubscriptionDataWriter_write(g_DCPSSubscription_writer,
                                                 (DDS_DCPSSubscription*) &st_data_used,
                                                 NULL) == DDS_RETCODE_ERROR) {
            // handle error
            Log_error("Send subscription topic failed.\n");
            return SDDS_RT_FAIL;
        }
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
}

# ifdef FEATURE_SDDS_LOCATION_TRACKING_ENABLED
rc_t
BuiltInTopicPublicationService_publishDCPSLocation(DeviceLocation_t* dev) {
    assert(dev);

    SDDS_DCPSLocation loc_data_used;
    DDS_ReturnCode_t ret;
    msec16_t remainingMSec;

    loc_data_used.pkey = dev->device;
    loc_data_used.device = dev->device;
    loc_data_used.x = dev->area.basicShape.vertex.x;
    loc_data_used.y = dev->area.basicShape.vertex.y;
    loc_data_used.z = dev->area.basicShape.vertex.z;
    loc_data_used.width = dev->area.basicShape.width;
    loc_data_used.length = dev->area.basicShape.length;
    loc_data_used.expiration = dev->expiration;
    loc_data_used.age = 0;

    ssw_rc_t ssw_ret = Time_remainMSec16(dev->time, &remainingMSec);
    if (ssw_ret == SDDS_SSW_RT_OK) {
        loc_data_used.age = -remainingMSec;
    }

    if (DDS_DCPSLocationDataWriter_write(g_DCPSLocation_writer,
                                         &loc_data_used,
                                         NULL) == DDS_RETCODE_ERROR) {
        // handle error
        Log_error("Send subscription topic failed.\n");
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}

static void
s_BuiltInTopicPublicationService_publishDCPSLocation() {
    List_t* devices = LocationTrackingService_getLocations();
    DeviceLocation_t* dev = (DeviceLocation_t*) devices->first_fn(devices);
    while (dev != NULL) {
        if (BuiltInTopicPublicationService_publishDCPSLocation(dev) != SDDS_RT_OK) {
            Log_error("Failed to publish DCPSLocation.\n");
        }
        dev = (DeviceLocation_t*) devices->next_fn(devices);
    }
}

# endif
#endif
