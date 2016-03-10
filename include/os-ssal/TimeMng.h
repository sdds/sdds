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

typedef uint16_t time16_t;
typedef int16_t msec16_t;
typedef uint16_t msecu16_t;
typedef uint32_t time32_t;
typedef int32_t msec32_t;
typedef uint32_t msecu32_t;

struct TimeStampSimple_struct {
    // use only the first 4 bit!
    uint8_t firstField;
    uint8_t secondField;
    uint8_t thirdField;
};
//typedef struct TimeStampSimple_struct TimeStampSimple_t;

rc_t
TimeMng_init(void);

rc_t
Time_getTime16(time16_t* time);
rc_t
Time_remainMSec16(time16_t* refTime, msec16_t* remainingMSec);

rc_t
Time_getTime32(time32_t* time);
rc_t
Time_remainMSec32(time32_t* refTime, msec32_t* remainingMSec);

#endif   /* ----- #ifndef TIMEMNG_T_INC  ----- */
