/*
 * =====================================================================================
 *
 *       Filename:  sDDS.h
 *
 *    Description:  dunno yet
 *
 *        Version:  1.0
 *        Created:  22.02.2010 15:31:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  SDDS_H_INC
#define  SDDS_H_INC

#include "sdds_types.h"

#ifdef _ECLIPSE_DEV_
#include "CONSTANTS.h"
#endif // _ECLIPSE_DEV_
// special types

rc_t
   sDDS_init (void);

#ifndef SDDS_BIT_ARRAY_MAX_OBJS
   #define SDDS_BIT_ARRAY_MAX_OBJS 25
#endif
typedef struct _BitArray_t BitArray_t;
#ifndef SDDS_DATA_READER_MAX_OBJS
   #define SDDS_DATA_READER_MAX_OBJS 64
#endif
typedef struct _DataReader_t DataReader_t;
typedef struct _Topic_t Topic_t;

#include "BitArray.h"
#include "DataReader.h"
#include "Log.h"
#include "Topic.h"

#endif   /* ----- #ifndef SDDS_H_INC  ----- */
