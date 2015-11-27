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

rc_t
   sDDS_init (void);

//  Forward declaration of classes
#ifndef SDDS_DATA_READER_MAX_OBJS
   #define SDDS_DATA_READER_MAX_OBJS 64
#endif
typedef struct _DataReader_t DataReader_t;
typedef struct _History_t History_t;
typedef struct _NetBuffRef_t NetBuffRef_t;
typedef struct _Sample_t Sample_t;
typedef struct _Topic_t Topic_t;

//  Class headers
#include "BitArray.h"
#include "DataReader.h"
//#include "History.h"
#include "Locator.h"
#include "Log.h"
#include "NetBuffRef.h"
#include "Topic.h"
#include "os-ssal/Task.h"

#endif   /* ----- #ifndef SDDS_H_INC  ----- */
