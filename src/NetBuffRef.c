/*
 * =====================================================================================
 *
 *       Filename:  NetBuffRef.c
 *
 *    Description:  Impl file for the Network Buffer Reference class
 *
 *        Version:  1.0
 *        Created:  24.02.2010 18:27:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "sDDS.h"

#ifdef UTILS_DEBUG
#include <stdio.h>
#endif

rc_t
NetBuffRef_init(NetBuffRef_t* _this) {
    Network_getFrameBuff(&(_this->frame_start));

    // get the buffer start?
    // TODO

    size_t start;
    Network_getPayloadBegin(&start);
    _this->buff_start = ((byte_t*) (_this->frame_start)) + start;
    _this->locators = List_initConcurrentLinkedList();
    NetBuffRef_renew(_this);

    return SDDS_RT_OK;
}

rc_t
NetBuffRef_renew(NetBuffRef_t* _this) {
    _this->subMsgCount = 0;
    _this->curPos = 0;
    _this->sendDeadline = 0;
    Locator_t *loc = (Locator_t*)_this->locators->first_fn(_this->locators);
    while (loc != NULL) {
        Locator_downRef(loc);
        loc = (Locator_t*)_this->locators->next_fn(_this->locators);
    }
    _this->locators->delete_all_fn(_this->locators);
    _this->curTopic = NULL;
    _this->curDomain = SDDS_DOMAIN_NIL;
    _this->bufferOverflow = false;

    return SDDS_RT_OK;
}

//#ifdef UTILS_DEBUG
void
NetBuffRef_print(NetBuffRef_t* _this) {

    printf("NetRefBuf Print:\n");
    printf("\ttopicid: %i\n", (_this->curTopic == NULL) ? 0 : _this->curTopic->id);
    printf("\tsubMsgCount: %i \n", _this->subMsgCount);
    printf("\tbuffPos: %i \n", _this->curPos);
    printf("\tNetFramesize: %i \n", (int)_this->frame_start->size);
    printf("\tBuffer: \n");

    printf("Byte:\t");
    for (int i = 0; i < _this->curPos; i++) {
        printf("  %2i ", i);
    }

    printf("\n\t");
    for (int i = 0; i < _this->frame_start->size; i++) {
        printf("0x%02x ", (uint8_t)(_this->buff_start[i]));
    }
    printf("\n");

}

void
NetBuffRef_print_subMsgs (NetBuffRef_t* _this) {
    int curPos = _this->curPos;
    subMsg_t readType = 0;
    topicid_t topic_id = 0;
    uint32_t seqNr = 0;

printf("  subMsgCount %d\n", _this->subMsgCount);

    for (int i=0; i<_this->subMsgCount; i++) {
        SNPS_evalSubMsg(_this, &readType);

        switch (readType) {
        case (SDDS_SNPS_SUBMSG_DOMAIN):
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_DOMAIN: 1 \n", i);
            break;
        case (SDDS_SNPS_SUBMSG_TOPIC):
            SNPS_readTopic(_this, &topic_id);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_TOPIC: %d \n", i, topic_id);
            break;
        case (SDDS_SNPS_SUBMSG_DATA):
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_DATA\n", i);
            break;
#ifdef SDDS_HAS_QOS_RELIABILITY
        case (SDDS_SNPS_SUBMSG_SEQNR):
            SNPS_readSeqNr(_this, (uint8_t*)&seqNr);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_SEQNR: %d \n", i, seqNr);
            break;
#endif
        case (SDDS_SNPS_SUBMSG_TS):
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_TS\n", i);
            break;
#ifdef SDDS_HAS_QOS_RELIABILITY
#   ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_ACK
        case (SDDS_SNPS_SUBMSG_ACKSEQ):
            SNPS_readAckSeq(_this, (uint8_t*)&seqNr);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_ACKSEQ: %d \n", i, seqNr);
            break;
#   endif
#   ifdef SDDS_HAS_QOS_RELIABILITY_KIND_RELIABLE_NACK
        case (SDDS_SNPS_SUBMSG_NACKSEQ):
            //SNPS_readNackSeq(_this, (uint8_t*)&seqNr);
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_NACKSEQ: %d \n", i, seqNr);
            break;
#   endif
#endif
        case (SDDS_SNPS_SUBMSG_STATUS):
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_STATUS\n", i);
            break;
        case (SDDS_SNPS_SUBMGS_ADDR):
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMGS_ADDR\n", i);
            break;
        case (SDDS_SNPS_SUBMSG_EXTENDED):
            SNPS_discardSubMsg(_this);
            printf("subMsg: %d, SDDS_SNPS_SUBMSG_EXTENDED\n", i);
            break;
        }
    }

    _this->curPos = curPos;
}


void
NetBuffRef_print_subMsgType (NetBuffRef_t* _this, subMsg_t type) {

    uint8_t curPos = _this->curPos;
    uint8_t subMsgCount = _this->subMsgCount;

    _this->curPos = 0;
    topicid_t topic = 0;
    uint32_t seqNr = 0;
    bool_t isRelevantTopic = 0;

    while (SNPS_gotoNextSubMsg(_this, SDDS_SNPS_SUBMSG_TOPIC) == SDDS_RT_OK){
        if (_this->curPos >= SDDS_NET_MAX_BUF_SIZE) {
            break;
        }

        SNPS_readTopic(_this, &topic);
        if (topic == 11) {
            isRelevantTopic = 1;
        }
    }

    _this->curPos = 0;
    _this->subMsgCount = subMsgCount;

    if (isRelevantTopic) {
        printf("    subMsgCount: %d\n", subMsgCount);

        while (SNPS_gotoNextSubMsg(_this, SDDS_SNPS_SUBMSG_TOPIC) == SDDS_RT_OK){
            if (_this->curPos >= SDDS_NET_MAX_BUF_SIZE) {
                break;
            }

            SNPS_readTopic(_this, &topic);
            printf("    netbuffer subMsg: %d \t byte: %d \t topic: %d\n", _this->subMsgCount, _this->curPos, topic);

        }

        _this->curPos = 0;
        _this->subMsgCount = subMsgCount;

        while (SNPS_gotoNextSubMsg(_this, type) == SDDS_RT_OK){
            if (_this->curPos >= SDDS_NET_MAX_BUF_SIZE) {
                break;
            }

#ifdef SDDS_HAS_QOS_RELIABILITY
            SNPS_readSeqNr(_this, (uint8_t*) &seqNr);
            printf("    netbuffer subMsg: %d \t byte: %d \t seqNr: %d\n", _this->subMsgCount, _this->curPos, seqNr);
#endif
        }
    }
    _this->curPos = curPos;
    _this->subMsgCount = subMsgCount;
}

//#endif
