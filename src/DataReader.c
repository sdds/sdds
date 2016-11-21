/*  =========================================================================
    DataReader - DDS DataReader representation, allows the application to
                 declare the data it wishes to receive.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#include "sDDS.h"

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
//  ---------------------------------------------------------------------------
//  Tries to take a sample from the data readers history. The provided
//  structure must match the de-serialized data for this topic. Return
//  SDDS_RT_OK if data available, otherwise SDDS_RT_NODATA.

void
DataReader_init(DataReader_t* self, uint8_t id, Topic_t* topic, On_Data_Avail_Listener listener) {
    assert(self);
    assert(topic);

    self->id = id;
    self->topic = topic;
    self->on_data_avail_listener = listener;
    self->fn_pushData = DataReader_s_pushData;
}

rc_t
DataReader_take_next_sample(DataReader_t* self, Data* data, DataInfo info) {
    assert(self);
    assert(data);
    (void) info;
    //  Check if buffer is provided
    if (*data) {
        Sample_t* sample = sdds_History_dequeue(DataReader_history(self));
        if (sample == NULL) {
            return SDDS_RT_NODATA;
        }
        (self->topic->Data_cpy)(*data, (Data) sample->data);
        //  TODO Sample infos
        return SDDS_RT_OK;
    }
    else {
        //  TODO Implement loan
        Log_error("No buffer for datasample is provided. Data is lost\n");
        return SDDS_RT_FAIL;
    }
}

On_Data_Avail_Listener
DataReader_on_data_avail_listener(DataReader_t* self) {
    return self->on_data_avail_listener;
}

rc_t
DataReader_set_on_data_avail_listener(DataReader_t* self,
                                      On_Data_Avail_Listener listener,
                                      const StatusMask sm) {
    self->on_data_avail_listener = listener;
    return SDDS_RT_OK;
}

unsigned int
DataReader_id(DataReader_t* self) {
    return self->id;
}

Topic_t*
DataReader_topic(DataReader_t* self) {
    return self->topic;
}

History_t*
DataReader_history(DataReader_t* self) {
    assert(self);
    return &self->history;
}

rc_t
DataReader_pushData(DataReader_t* self, NetBuffRef_t* buff) {
	assert(self);
	assert(buff);
    return self->fn_pushData(self, buff);
}

rc_t
DataReader_s_pushData(DataReader_t* self, NetBuffRef_t* buff) {
    History_t* history = DataReader_history(self);
    Topic_t* topic = buff->curTopic;
    Locator_t* loc = (Locator_t*) buff->locators->first_fn(buff->locators);
    rc_t ret;

#   ifdef SDDS_HAS_QOS_RELIABILITY
    ret = sdds_History_enqueue(history, topic, &topic->incomingSample, topic->incomingSample.seqNr);
    if (ret == SDDS_RT_FAIL) {
        Log_warn("Can't parse data: Discard submessage\n");
        SNPS_discardSubMsg(buff);
        return SDDS_RT_FAIL;
    }

#       if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)

#           ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
                bool_t seqNrsAreMissing = 0;
                uint8_t indexOfLoc = 0;

                if(topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_NACK){
                    // check, if sending of NACK msgs are necessary

                    for (int index=0; index<SDDS_QOS_RELIABILITY_MAX_TOPIC_PARTICIPANTS; index++){
                        if (history->qos_locator[index] == loc){
                            indexOfLoc = index;
                            break;
                        }
                    }

                    for (int index=0; index<SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                        if (history->missingSeqNrSlotIsUsed[indexOfLoc][index] == 1) {
                            seqNrsAreMissing = 1;
                            break;
                        }
                    }
                }


                if(topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK || seqNrsAreMissing){
#           else
                if(topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK){
#           endif

                    DataWriter_mutex_lock();
                    Topic_addRemoteDataSink(topic, loc, 0, ACTIVE);
                    Locator_upRef(loc);

                    List_t* subscribers = topic->dsinks.list;


#           ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK
                    if(topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK){
                        NetBuffRef_t* out_buffer = find_free_buffer(subscribers);
                        domainid_t domain = topic->domain;
                        if (out_buffer->curDomain != domain) {
                            SNPS_writeDomain(out_buffer, domain);
                            out_buffer->curDomain = domain;
                        }

                        if (out_buffer->curTopic != topic) {
                            SNPS_writeTopic(out_buffer, topic->id);
                            out_buffer->curTopic = topic;
                        }

                        if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC){
                            SNPS_writeAckSeq(out_buffer, topic->incomingSample.seqNr);
#               if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
                        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL){
                            SNPS_writeAck(out_buffer);
                            SNPS_writeSeqNrSmall(out_buffer, topic->incomingSample.seqNr);
#               endif
#               if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
                        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG){
                            SNPS_writeAck(out_buffer);
                            SNPS_writeSeqNrBig(out_buffer, topic->incomingSample.seqNr);
#               endif
#               if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
                        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE){
                            SNPS_writeAck(out_buffer);
                            SNPS_writeSeqNrHUGE(out_buffer, topic->incomingSample.seqNr);
#               endif
                        }

#               ifdef SDDS_QOS_LATENCYBUDGET
#                   if SDDS_QOS_DW_LATBUD < 65536
                        ret = Time_getTime16(&out_buffer->sendDeadline);
#                   else
                        ret = Time_getTime32(&out_buffer->sendDeadline);
#                   endif
                        out_buffer->latBudDuration = self->qos.latBudDuration;
                        Log_debug("sendDeadline: %u\n", out_buffer->sendDeadline);
#               endif
                        checkSending(out_buffer);
                    } // end of confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK
#           endif // end of SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK


#           ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
                    if(topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_NACK){
                        for (int index=0; index<SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                            if (history->missingSeqNrSlotIsUsed[indexOfLoc][index] == 0) {
                                continue;
                            }

                            NetBuffRef_t* out_buffer = find_free_buffer(subscribers);
                            domainid_t domain = topic->domain;
                            if (out_buffer->curDomain != domain) {
                                SNPS_writeDomain(out_buffer, domain);
                                out_buffer->curDomain = domain;
                            }

                            if (out_buffer->curTopic != topic) {
                                SNPS_writeTopic(out_buffer, topic->id);
                                out_buffer->curTopic = topic;
                            }


                            if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC){
                                SNPS_writeNackSeq(out_buffer, history->missingSeqNrsByLoc[indexOfLoc][index]);
#               if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
                            } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL){
                                SNPS_writeNack(out_buffer);
                                SNPS_writeSeqNrSmall(out_buffer, history->missingSeqNrsByLoc[indexOfLoc][index]);
#               endif
#               if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
                            } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG){
                                SNPS_writeNack(out_buffer);
                                SNPS_writeSeqNrBig(out_buffer, history->missingSeqNrsByLoc[indexOfLoc][index]);
#               endif
#               if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
                            } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE){
                                SNPS_writeNack(out_buffer);
                                SNPS_writeSeqNrHUGE(out_buffer, history->missingSeqNrsByLoc[indexOfLoc][index]);
#               endif
                            }

#               ifdef SDDS_QOS_LATENCYBUDGET
#                   if SDDS_QOS_DW_LATBUD < 65536
                            ret = Time_getTime16(&out_buffer->sendDeadline);
#                   else
                            ret = Time_getTime32(&out_buffer->sendDeadline);
#                   endif
                            out_buffer->latBudDuration = self->qos.latBudDuration;
                            Log_debug("sendDeadline: %u\n", out_buffer->sendDeadline);
#               endif

                            checkSending(out_buffer);

                        } // end of for all missingSamples
                    } // end of confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_NACK
#           endif // end of SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK


                    DataWriter_mutex_unlock();
                }  // end of (topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK) || missingSamples
#       endif // end of SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK/NACK
#    else // else of: if SDDS_HAS_QOS_RELIABILITY

    ret = sdds_History_enqueue(history, topic, &topic->incomingSample);
    if (ret == SDDS_RT_FAIL) {
        Log_warn("Can't parse data: Discard submessage\n");
        SNPS_discardSubMsg(buff);
        return SDDS_RT_FAIL;
    }
#    endif // end of: if SDDS_HAS_QOS_RELIABILITY

    return SDDS_RT_OK;
}

#endif
