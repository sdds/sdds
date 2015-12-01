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

#if defined SDDS_QOS_RELIABILITY
   // QoS_Reliable options
   #define SDDS_QOS_RELIABILITY_KIND_BESTEFFORT 0
   #define SDDS_QOS_RELIABILITY_KIND_RELIABLE 1
   #define SDDS_QOS_RELIABILITY_SEQSIZE_NORMAL 0
   #define SDDS_QOS_RELIABILITY_SEQSIZE_SMALL 1
   #define SDDS_QOS_RELIABILITY_SEQSIZE_BIG 2
   #define SDDS_QOS_RELIABILITY_SEQSIZE_HUGE 3
   // QoS_Reliable settings
   #define SDDS_QOS_RELIABILITY_KIND SDDS_QOS_RELIABILITYKIND_BESTEFFORT
   #define SDDS_QOS_RELIABILITY_SEQSIZE SDDS_QOS_RELIABILITY_SEQSIZE_NORMAL
   // defining datatype
   #if (SDDS_QOS_RELIABILITY_SEQSIZE < SDDS_QOS_RELIABILITY_SEQSIZE_BIG)
      #define seqNr_t uint8_t
   #elif (SDDS_QOS_RELIABILITY_SEQSIZE == SDDS_QOS_RELIABILITY_SEQSIZE_BIG)
      #define seqNr_t uint16_t
   #else
      #define seqNr_t uint32_t
   #endif
#endif

struct SourceQos_t{
#ifdef SDDS_QOS_LATENCYBUDGET
#if SDDS_QOS_DW_LATBUD < 65536
    msecu16_t latBudDuration;
#else
    msecu32_t latBudDuration;
#endif
#endif

#if defined SDDS_QOS_RELIABILITY
    seqNr_t seqNr;
#endif

    uint8_t subMsgNeeded;
};
typedef struct SourceQos_t* SourceQos;


#endif   /* ----- #ifndef QOS_INC  ----- */
