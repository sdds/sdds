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

#ifndef SDDS_QOS_LATBUD_COMM
#define SDDS_QOS_LATBUD_COMM 0
#endif

#ifndef SDDS_QOS_LATBUD_READ
#define SDDS_QOS_LATBUD_READ 0
#endif

#ifndef SDDS_QOS_DW_LATBUD
#define SDDS_QOS_DW_LATBUD 0
#endif

struct SourceQos_t{
#if SDDS_QOS_DW_LATBUD < 65536
    msecu16_t latBudDuration;
#else
    msecu32_t latBudDuration;
#endif
    uint8_t subMsgNeeded;
};
typedef struct SourceQos_t* SourceQos;


#endif   /* ----- #ifndef QOS_INC  ----- */
