/*
 * =====================================================================================
 *
 *       Filename:  DataSource.c
 *
 *    Description:  Impl for the datasource class
 *
 *        Version:  1.0
 *        Created:  22.02.2010 18:09:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "sDDS.h"

#ifndef SDDS_PLATFORM_autobest
#include <stdlib.h>
#endif
#include <string.h>

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
// participant ID is defined in src/BuiltinTopic.c
extern SSW_NodeID_t BuiltinTopic_participantID;
#endif


struct _InstantSender_t {
    NetBuffRef_t highPrio;
    NetBuffRef_t out[SDDS_NET_MAX_OUT_QUEUE];
};
typedef struct _InstantSender_t InstantSender_t;

struct _DataSource_t {
#if SDDS_MAX_DATA_WRITERS > 0
#   ifdef SDDS_HAS_QOS_RELIABILITY
    Reliable_DataWriter_t writers[SDDS_MAX_DATA_WRITERS];
#   else
    DataWriter_t writers[SDDS_MAX_DATA_WRITERS];
#   endif
#endif
    InstantSender_t sender;
    unsigned int remaining_datawriter : 4;
};

static DataSource_t dsStruct;
static DataSource_t* self = &dsStruct;

void print_Pointer() {
    for (int index=0; index < SDDS_NET_MAX_OUT_QUEUE; index++) {
        Log_debug("%d p: (%d) %p\n", __LINE__, index, self->sender.out[index].locators->first_fn);
    }
}

rc_t
DataSource_init(void) {
    self->remaining_datawriter = SDDS_MAX_DATA_WRITERS;

    //  Init instant sender
    //  The init method alloc a frame buffer and adds it to the structure
    NetBuffRef_init(&(self->sender.highPrio));

    for (uint8_t i=0; i<SDDS_NET_MAX_OUT_QUEUE; i++) {
        NetBuffRef_init(&(self->sender.out[i]));
    }

    return DataWriter_init();
}

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED
rc_t
DataSource_getTopic(DDS_DCPSSubscription* st, topicid_t id, Topic_t** topic) {
    int index;
    for (index = 0; index < (SDDS_MAX_DATA_WRITERS - self->remaining_datawriter);
         index++) {
#   ifdef SDDS_HAS_QOS_RELIABILITY
        DataWriter_t* dw = (DataWriter_t*) &(self->writers[index]);
        if (dw->topic->id == id) {
                if (st != NULL) {
                st->key = dw->id;
#      ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
                st->participant_key = BuiltinTopic_participantID;
#      endif
                st->topic_id = dw->topic->id;
            }
            if (topic != NULL) {
                *topic = dw->topic;
            }
            return SDDS_RT_OK;
        }
#   else
        if ((self->writers[index].topic->id == id)) {
            if (st != NULL) {
                st->key = self->writers[index].id;
#     ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
                st->participant_key = BuiltinTopic_participantID;
#     endif
                st->topic_id = self->writers[index].topic->id;
            }
            if (topic != NULL) {
                *topic = self->writers[index].topic;
            }
            return SDDS_RT_OK;
        }
#   endif
    }

    return SDDS_RT_FAIL;
}
#endif

#ifdef FEATURE_SDDS_DISCOVERY_ENABLED
rc_t
DataSource_getDataWrites(DDS_DCPSPublication* pt, int* len) {
    int index = 0;
    *len = 0;

    for (index = 0; index < (SDDS_MAX_DATA_WRITERS - self->remaining_datawriter); index++) {
#   ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
#       ifdef SDDS_HAS_QOS_RELIABILITY
        DataWriter_t* dw = (DataWriter_t*) &(self->writers[index]);
        if (!BuildinTopic_isBuiltinTopic(dw->topic->id,
                                         dw->topic->domain)) {

        pt[*len].key = dw->id;
#            ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
        pt[*len].participant_key = BuiltinTopic_participantID;
#            endif
        pt[*len].topic_id = dw->topic->id;
#       else
        if (!BuildinTopic_isBuiltinTopic(self->writers[index].topic->id,
                                         self->writers[index].topic->domain)) {

        pt[*len].key = self->writers[index].id;
#           ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
        pt[*len].participant_key = BuiltinTopic_participantID;
#           endif
        pt[*len].topic_id = self->writers[index].topic->id;
#       endif
#   endif

        (*len)++;
#   ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
    }
#   endif
    }
    return SDDS_RT_OK;
}
#endif

#if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
Reliable_DataWriter_t*
DataSource_DataWriter_by_topic(topicid_t id){
    for (int index = 0; index < SDDS_MAX_DATA_WRITERS; index++){
        if (dsStruct.writers[index].dw.topic->id == id){
            return &dsStruct.writers[index];
        }
    }

    return NULL;
}
#endif

#if SDDS_MAX_DATA_WRITERS > 0
DataWriter_t*
DataSource_create_datawriter(Topic_t* topic, Qos qos,
                             Listener list, StatusMask mask) {
    (void) qos;
    (void) list;
    (void) mask;

    DataWriter_t* dw = NULL;

    if (self->remaining_datawriter == 0) {
        return NULL;
    }
    dw = (DataWriter_t*) &(self->writers[SDDS_MAX_DATA_WRITERS - self->remaining_datawriter]);

    dw->topic = topic;
    dw->id = (SDDS_MAX_DATA_WRITERS - self->remaining_datawriter);
    self->remaining_datawriter--;

#   ifdef SDDS_QOS_LATENCYBUDGET
    dw->qos.latBudDuration = SDDS_QOS_DW_LATBUD - SDDS_QOS_LATBUD_COMM - SDDS_QOS_LATBUD_READ;
#   endif
#   ifdef SDDS_HAS_QOS_RELIABILITY
    ((Reliable_DataWriter_t*) dw)->seqNr = 0;
#   endif
    return dw;
}
#endif // SDDS_MAX_DATA_WRITERS

NetBuffRef_t*
find_free_buffer(List_t* topic_locators) {
    assert (topic_locators);

    NetBuffRef_t* free_buffer = NULL;
    bool_t same_locators = false;
    //  Try to find a buffer that has all topic locators attached to it

    for (uint8_t index = 0; index < SDDS_NET_MAX_OUT_QUEUE; index++) {
        NetBuffRef_t* send_buffer = &self->sender.out[index];
        List_t* send_buff_locators = send_buffer->locators;
        if (send_buff_locators) {
            same_locators = true;
            Locator_t* topic_locator = topic_locators->first_fn(topic_locators);
            while (topic_locator) {
                if (Locator_contains(send_buff_locators, topic_locator) != SDDS_RT_OK) {
                    same_locators = false;
                    break;
                }
                topic_locator = topic_locators->next_fn(topic_locators);
            }
            if (same_locators) {
                free_buffer = &(self->sender.out[index]);
                break;
            }
        }
    }

    //  Try to find an empty buffer
    if (free_buffer == NULL) {
        for (uint8_t index = 0; index <SDDS_NET_MAX_OUT_QUEUE; index++) {
            if (self->sender.out[index].curPos == 0) {
                free_buffer = &(self->sender.out[index]);
                break;
            }
        }
    }
    //  If no buffer could be obtained use the high prio instead
    if (free_buffer == NULL) {
        free_buffer = &(self->sender.highPrio);
        free_buffer->curPos = 0;
    }
    //  Initialize an empty buffer
    if (free_buffer->curPos == 0) {
        SNPS_initFrame(free_buffer);
    }
    if (same_locators == false) {
        //  Append locators to the obtained free buffer
        Locator_t* loc = (Locator_t*) topic_locators->first_fn(topic_locators);
        while (loc != NULL) {
            if (Locator_contains(free_buffer->locators, loc) != SDDS_RT_OK) {
                if (free_buffer->locators->add_fn(free_buffer->locators, loc) == SDDS_RT_OK) {
                    Locator_upRef(loc);
                }
            }
            loc = (Locator_t*) topic_locators->next_fn(topic_locators);
        }
    }
    return free_buffer;
}
