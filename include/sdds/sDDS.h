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
sDDS_init(void);

//  Forward declaration of classes
#ifndef SDDS_DATA_READER_MAX_OBJS
#define SDDS_DATA_READER_MAX_OBJS 64
#endif
typedef struct _DataReader_t DataReader_t;
typedef struct _DataWriter_t DataWriter_t;
typedef struct _History_t History_t;
typedef struct _NetBuffRef_t NetBuffRef_t;
typedef struct _Locator_t Locator_t;
typedef struct SourceQos_t SourceQos_t;
typedef struct _Sample_t Sample_t;
typedef struct _Topic_t Topic_t;
typedef struct TimeStampSimple_struct TimeStampSimple_t;

//  Abstraction
#include "os-ssal/Task.h"
#include "os-ssal/TimeMng.h"

//  Class headers
#include "BitArray.h"
#include "Qos.h"
#include "Marshalling.h"
#include "SNPS.h"
#include "NetBuffRef.h"
#include "History.h"
#include "DataReader.h"
#include "DataWriter.h"
#include "Locator.h"
#include "Log.h"
#include "Sample.h"
#include "Topic.h"

#endif   /* ----- #ifndef SDDS_H_INC  ----- */
