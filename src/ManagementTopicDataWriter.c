/*
 * ManagementTopicDataWriter.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "ManagementTopicDataWriter.h"

#ifdef FEATURE_SDDS_MANAGEMENT_TOPIC_ENABLED

rc_t
checkSendingToParticipant(NetBuffRef_t* buf, Locator_t* addr) {
    rc_t ret;
    // update header
    SNPS_updateHeader(buf);

//    ret = buf->locators->delete_all_fn(buf->locators);
//    if (ret != SDDS_RT_OK) {
//        return SDDS_RT_FAIL;
//    }

    ret = buf->locators->add_fn(buf->locators, addr);
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }

    ret = Network_send(buf);
    if (ret != SDDS_RT_OK) {
        Log_error("Network_send failed\n");
        NetBuffRef_renew(buf);
        return SDDS_RT_FAIL;
    }

    NetBuffRef_renew(buf);

    return ret;
}

rc_t
ManagementTopicDataWriter_writeToParticipant(DataWriter_t* self,
                                             Data data,
                                             void* handle,
                                             Locator_t* addr
                                             ) {
    assert(self);
    (void) handle;

    DataWriter_mutex_lock();
    Topic_t* topic = self->topic;
    List_t* subscribers = topic->dsinks.list;
    NetBuffRef_t* out_buffer = find_free_buffer(subscribers);
    rc_t ret;

    domainid_t domain = topic->domain;
    if (out_buffer->curDomain != domain) {
        SNPS_writeDomain(out_buffer, domain);
        out_buffer->curDomain = domain;
    }

    if (out_buffer->curTopic != topic) {
        SNPS_writeTopic(out_buffer, topic->id);
        out_buffer->curTopic = topic;
    }

#ifdef SDDS_HAS_QOS_RELIABILITY

    if (self->topic->reliabilityKind > 0){ // relevant topic for qos reliability

        Reliable_DataWriter_t* dw_reliable_p = (Reliable_DataWriter_t*)self;
        bool_t newSampleHasSlot = 0;


#   if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK) || defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK)
        if (self->topic->reliabilityKind == SDDS_QOS_RELIABILITY_KIND_RELIABLE) { // relevant topic has qos reliability_reliable

            // check, if sample is already in acknowledgement-list
            bool_t isAlreadyInQueue = 0;


            for (int index = 0; index < SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                if ((dw_reliable_p->samplesToKeep[index].isUsed == 1)
                &&  (dw_reliable_p->samplesToKeep[index].seqNr == dw_reliable_p->seqNr) ) {
                    isAlreadyInQueue = 1;
                    break;
                }
            }

            if (isAlreadyInQueue == 0) {
                // add current data of sample to acknowledgement-queue for possible re-sending
                time32_t currentTime = 0;
                Time_getTime32(&currentTime);

                // find free/replaceable slot in acknowledgement-list
                // if no slot can be found, drop new data
                for (int index = 0; index < SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                    if(dw_reliable_p->samplesToKeep[index].isUsed == 0
                    || (dw_reliable_p->samplesToKeep[index].timeStamp + self->topic->max_blocking_time) < currentTime ) {

                        dw_reliable_p->samplesToKeep[index].isUsed = 1;
                        topic->Data_cpy(dw_reliable_p->samplesToKeep[index].data, data);
                        dw_reliable_p->samplesToKeep[index].seqNr = dw_reliable_p->seqNr;
                        dw_reliable_p->samplesToKeep[index].timeStamp = currentTime;
                        newSampleHasSlot = 1;
                        break;
                    }
                }
            }


#      if defined (SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK)
            if (self->topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK) {
            // send every sample which is not yet acknowledged
                for (int index = 0; index < SDDS_QOS_RELIABILITY_RELIABLE_SAMPLES_SIZE; index++) {
                    if(dw_reliable_p->samplesToKeep[index].isUsed != 0) {

                        if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC) {
                            SNPS_writeSeqNr(out_buffer, dw_reliable_p->samplesToKeep[index].seqNr);

#           if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
                        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL) {
                            SNPS_writeSeqNrSmall(out_buffer, dw_reliable_p->samplesToKeep[index].seqNr);
#           endif
#           if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
                        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG) {
                            SNPS_writeSeqNrBig(out_buffer, dw_reliable_p->samplesToKeep[index].seqNr);
#           endif
#           if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
                        } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE) {
                            SNPS_writeSeqNrHUGE(out_buffer, dw_reliable_p->samplesToKeep[index].seqNr);
#           endif
                        }

                        if (SNPS_writeData(out_buffer, topic->Data_encode, dw_reliable_p->samplesToKeep[index].data) != SDDS_RT_OK) {
                            Log_error("(%d) SNPS_writeData failed\n", __LINE__);
#           ifdef SDDS_QOS_LATENCYBUDGET
                            out_buffer->bufferOverflow = true;
#           endif
                        }

                    }
                } // end of for all samples in samplesToKeep
            } // end of confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK
#       endif // end of ACK

        } // end of relevant topic has qos reliability_reliable
#   endif // end of if SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK || NACK

#   ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
        if((self->topic->reliabilityKind == SDDS_QOS_RELIABILITY_KIND_BESTEFFORT)
        || (self->topic->confirmationtype == SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_NACK && newSampleHasSlot) )
#   else
        if (self->topic->reliabilityKind == SDDS_QOS_RELIABILITY_KIND_BESTEFFORT)
#   endif
        {

            if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC){
                SNPS_writeSeqNr(out_buffer, dw_reliable_p->seqNr);
#       if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_SMALL
            } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL){
                SNPS_writeSeqNrSmall(out_buffer, dw_reliable_p->seqNr);
#       endif
#       if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE >= SDDS_QOS_RELIABILITY_SEQSIZE_BIG
            } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG){
                SNPS_writeSeqNrBig(out_buffer, dw_reliable_p->seqNr);
#       endif
#       if SDDS_SEQNR_BIGGEST_TYPE_BITSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE
            } else if (topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_HUGE){
                SNPS_writeSeqNrHUGE(out_buffer, dw_reliable_p->seqNr);
#       endif
            }

        }


        if (self->topic->reliabilityKind == SDDS_QOS_RELIABILITY_KIND_RELIABLE ) {
            if (newSampleHasSlot) {
                if (self->topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC) {
                    dw_reliable_p->seqNr = (dw_reliable_p->seqNr + 1) & 0x0F;
                } else if (self->topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL){
                    dw_reliable_p->seqNr = (dw_reliable_p->seqNr + 1) & 0xFF;
                } else if (self->topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG){
                    dw_reliable_p->seqNr = (dw_reliable_p->seqNr + 1) & 0xFFFF;
                }else{
                    dw_reliable_p->seqNr++;
                }
            }
        } else {
            if (self->topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BASIC){
                dw_reliable_p->seqNr = (dw_reliable_p->seqNr + 1) & 0x0F;
            } else if (self->topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_SMALL){
                dw_reliable_p->seqNr = (dw_reliable_p->seqNr + 1) & 0xFF;
            } else if (self->topic->seqNrBitSize == SDDS_QOS_RELIABILITY_SEQSIZE_BIG){
                dw_reliable_p->seqNr = (dw_reliable_p->seqNr + 1) & 0xFFFF;
            }else{
                dw_reliable_p->seqNr++;
            }
        }

    } // end of topic is relevant for reliability

#endif // end if SDDS_HAS_QOS_RELIABILITY

#ifdef SDDS_HAS_QOS_RELIABILITY
#   ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
        if (self->topic->confirmationtype != SDDS_QOS_RELIABILITY_CONFIRMATIONTYPE_ACK)
#   else
        if (self->topic->reliabilityKind != SDDS_QOS_RELIABILITY_KIND_RELIABLE)
#   endif
        {
#endif
            ret = SNPS_writeData(out_buffer, topic->Data_encode, data);
            if (ret != SDDS_RT_OK) {
                Log_error("(%d) SNPS_writeData failed\n", __LINE__);
            }
#ifdef SDDS_HAS_QOS_RELIABILITY
        }
#endif

    Log_debug("writing to domain %d and topic %d \n", topic->domain, topic->id);

    ret = checkSendingToParticipant(out_buffer, addr);

    DataWriter_mutex_unlock();
    //  Caller doesn't understand different return codes but FAIL and OK
    if (ret != SDDS_RT_OK) {
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
} // end of DataWriter_write

#endif
