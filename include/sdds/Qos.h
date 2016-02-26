/*
 * =====================================================================================
 *
 *       Filename:  Qos.h
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

#ifndef  QOS_INC
#define  QOS_INC
#include "sdds_types.h"
#include "os-ssal/TimeMng.h"

#ifndef SDDS_QOS_HISTORY_BLOCKING_TIME
#define SDDS_QOS_HISTORY_BLOCKING_TIME 5000
#endif

#ifdef SDDS_QOS_LATENCYBUDGET
#ifndef SDDS_QOS_LATBUD_COMM
#define SDDS_QOS_LATBUD_COMM 0
#endif

#ifndef SDDS_QOS_LATBUD_READ
#define SDDS_QOS_LATBUD_READ 0
#endif

#ifndef SDDS_QOS_DW_LATBUD
#define SDDS_QOS_DW_LATBUD 0
#endif
#endif

#ifdef SDDS_HAS_QOS_RELIABILITY
#define SDDS_QOS_RELIABILITY_KIND_BESTEFFORT 1
#define SDDS_QOS_RELIABILITY_KIND_RELIABLE_ACK 2
#define SDDS_QOS_RELIABILITY_KIND_RELIABLE_NACK 3
#define SDDS_QOS_RELIABILITY_SEQSIZE_BASIC 4
#define SDDS_QOS_RELIABILITY_SEQSIZE_SMALL 8
#define SDDS_QOS_RELIABILITY_SEQSIZE_BIG 16
#define SDDS_QOS_RELIABILITY_SEQSIZE_HUGE 32
#endif

struct SourceQos_t {
#ifdef SDDS_QOS_LATENCYBUDGET
#if SDDS_QOS_DW_LATBUD < 65536
    msecu16_t latBudDuration;
#else
    msecu32_t latBudDuration;
#endif
#endif

    uint8_t subMsgNeeded;
};

#endif   /* ----- #ifndef QOS_INC  ----- */
