/*
 * =====================================================================================
 *
 *       Filename:  DDS_DCPS.h
 *
 *    Description:  Header for the DCPS API
 *
 *        Version:  1.0
 *        Created:  19.02.2010 13:15:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  DDS_DCPS_H_INC
#define  DDS_DCPS_H_INC

#include "sdds/sDDS.h"
#include "sdds/sdds_types.h"

typedef int16_t		DDS_short;
typedef int32_t		DDS_long;
typedef int64_t		DDS_long_long;
typedef uint16_t	DDS_unsigned_short;
typedef uint32_t	DDS_unsigned_long;
typedef uint64_t	DDS_unsigned_long_long;
typedef float32_t	DDS_float;
typedef float64_t	DDS_double;
//typedef float128_t	DDS_long_double;
typedef char_t		DDS_char;
typedef uint8_t		DDS_octet;
typedef bool_t		DDS_boolean;
typedef DDS_char*	DDS_string;

typedef int16_t		DDS_ReturnCode_t;

#define DDS_RETCODE_OK                                  0
#define DDS_RETCODE_ERROR                               1
#define DDS_RETCODE_UNSUPPORTED                         2
#define DDS_RETCODE_BAD_PARAMETER                       3
#define DDS_RETCODE_PRECONDITION_NOT_MET                4
#define DDS_RETCODE_OUT_OF_RESOURCES                    5
#define DDS_RETCODE_NOT_ENABLED                         6
#define DDS_RETCODE_IMMUTABLE_POLICY                    7
#define DDS_RETCODE_INCONSISTENT_POLICY                 8
#define DDS_RETCODE_ALREADY_DELETED                     9
#define DDS_RETCODE_TIMEOUT                             10
#define DDS_RETCODE_NO_DATA                             11
#define DDS_RETCODE_ILLEGAL_OPERATION                   12


/**
 * Default handle to DDS objects. Shall point to the implemention class.
 **/
typedef void* DDS_Object;
typedef void* DDS_InstanceHandle_t;
typedef void* DDS_sample;

typedef DDS_Object DDS_DataWriter;
typedef DDS_Object DDS_Publisher;
typedef DDS_Object DDS_Topic;
typedef DDS_Object DDS_TypeSupport;

typedef DDS_Object DDS_DataReader;
typedef DDS_Object DDS_Subscriber;
typedef DDS_Object DDS_TopicDescription;

struct DDS_SampleInfo_t {
    DDS_long nope;
};
typedef struct DDS_SampleInfo_t DDS_SampleInfo;

// TEMP
typedef DDS_Object DDS_StatusMask;

struct DDS_DataWriterQos_t {
    DDS_long nope;
};
typedef struct DDS_DataWriterQos_t DDS_DataWriterQos;

struct DDS_DataReaderQos_t {
    DDS_long nope;
};
typedef struct DDS_DataReaderQos_t DDS_DataReaderQos;

struct DDS_DataWriterListener {
    DDS_long nope;
};

struct DDS_DataReaderListener{
    DDS_long nope;
};

#define DDS_init() sDDS_init()


DDS_DataWriter DDS_Publisher_create_datawriter (
    DDS_Publisher _this,
    const DDS_Topic a_topic,
    const DDS_DataWriterQos *qos,
    const struct DDS_DataWriterListener *a_listener,
    const DDS_StatusMask mask);

DDS_DataReader DDS_Subscriber_create_datareader (
    DDS_Subscriber _this,
    const DDS_TopicDescription a_topic,
    const DDS_DataReaderQos *qos,
    const struct DDS_DataReaderListener *a_listener,
    const DDS_StatusMask mask);

#endif   /* ----- #ifndef DDS_DCPS_H_INC  ----- */
