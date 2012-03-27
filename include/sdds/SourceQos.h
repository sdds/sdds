/*
 * =====================================================================================
 *
 *       Filename:  SourceQos.h
 *
 *    Description:  structure to hold the qos properties regarding a dw
 *
 *        Version:  1.0
 *        Created:  25.02.2010 15:10:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  SOURCEQOS_INC
#define  SOURCEQOS_INC
#include "sdds_types.h"
#include "TimeMng.h"

#define SDDS_SOURCEQOS_SM_TSSIMPLE 0x01
#define SDDS_SOURCEQOS_SM_SEQNESIMPLE 0x02
#define SDDS_SOURCEQOS_SM_ACK 0x04
#define SDDS_SOURCEQOS_SM_NACK 0x08

struct SourceQos_t{
    msec_t latBudDuration;
    uint8_t subMsgNeeded;
};
typedef struct SourceQos_t* SourceQos;
#endif   /* ----- #ifndef SOURCEQOS_INC  ----- */
