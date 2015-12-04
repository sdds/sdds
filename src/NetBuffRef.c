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

    NetBuffRef_renew(_this);

    return SDDS_RT_OK;
}

rc_t
NetBuffRef_renew(NetBuffRef_t* _this) {
    _this->subMsgCount = 0;
    _this->curPos = 0;
    _this->sendDeadline = 0;
    _this->addr = NULL;
    _this->curTopic = NULL;
    _this->curDomain = SDDS_DOMAIN_NIL;

    return SDDS_RT_OK;
}

#ifdef UTILS_DEBUG
void
NetBuffRef_print(NetBuffRef_t* _this) {
    printf("NetRefBuf Print:\n");
    printf("\ttopicid: %i\n", (_this->curTopic == NULL) ? 0 : _this->curTopic->id);
    printf("\tsubMsgCount: %i \n", _this->subMsgCount);
    printf("\tbuffPos: %i \n", _this->curPos);
    printf("\tNetFramesize: %i \n", _this->frame_start->size);
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
#endif
