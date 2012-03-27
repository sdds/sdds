/*
 * =====================================================================================
 *
 *       Filename:  TimeMng.h
 *
 *    Description:  class to handle all aspects regarding time
 *
 *        Version:  1.0
 *        Created:  24.02.2010 14:10:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  TIMEMNG_T_INC
#define  TIMEMNG_T_INC

#include "sdds_types.h"

typedef uint16_t pointInTime_t;
typedef int16_t msec_t;

struct TimeStampSimple_struct{
    // use only the first 4 bit!
    uint8_t firstField;
    uint8_t secondField;
    uint8_t thirdField;
};
typedef struct TimeStampSimple_struct TimeStampSimple_t;

rc_t Time_getCurTime(pointInTime_t* curTime);
rc_t Time_remainMSec(pointInTime_t* refTime, msec_t* remainingMSec);

#endif   /* ----- #ifndef TIMEMNG_T_INC  ----- */
