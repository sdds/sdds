/*
 * FilteredDataReader.c
 *
 *  Created on: Aug 20, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "LocationFilteredTopic.h"
#include "FilteredDataReader.h"

#ifdef FEATURE_SDDS_LOCATION_FILTER_ENABLED

static rc_t
s_pushData(DataReader_t* self, NetBuffRef_t* buff);

void
FilteredDataReader_init(FilteredDataReader_t* self, uint8_t id, LocationFilteredTopic_t* topic, On_Data_Avail_Listener listener) {
    assert(self);
    assert(topic);

    self->dataReader.id = id;
    self->dataReader.topic = topic->contentFilteredTopic.associatedTopic;
    self->dataReader.on_data_avail_listener = listener;
    self->dataReader.fn_pushData = s_pushData;
    self->locationFilteredTopic = topic;
}

static rc_t
s_pushData(DataReader_t* self, NetBuffRef_t* buff) {
    History_t* history = DataReader_history(self);
    Topic_t* topic = buff->curTopic;
    Locator_t* loc = (Locator_t*) buff->locators->first_fn(buff->locators);
    rc_t ret;

#   ifdef SDDS_HAS_QOS_RELIABILITY
    ret = sdds_History_enqueue(history, topic, &topic->incomingSample, topic->incomingSample.seqNr, buff);
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
                    Topic_addRemoteDataSink(topic, loc);
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

    FilteredDataReader_t* fdr = (FilteredDataReader_t*) self;
    LocationFilteredTopic_t* locationFilteredTopic = fdr->locationFilteredTopic;

    ret = LocationFilteredTopic_evalSample(locationFilteredTopic, (Data) topic->incomingSample.data);

    if (ret == SDDS_RT_OK) {
        ret = sdds_History_enqueue(history, topic, &topic->incomingSample, buff);
        if (ret == SDDS_RT_FAIL) {
            Log_warn("Can't parse data: Discard submessage\n");
            SNPS_discardSubMsg(buff);
            return SDDS_RT_FAIL;
        }
    }
#		ifdef UTILS_DEBUG
    else {
    	Log_debug("Filter doesn't match.\n");
    }
#		endif
#   endif // end of: if SDDS_HAS_QOS_RELIABILITY

    return SDDS_RT_OK;
}

#endif
