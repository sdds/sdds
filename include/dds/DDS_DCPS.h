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
typedef char_t  	DDS_string;

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

typedef enum {
    DDS_NOT_REJECTED,
    DDS_REJECTED_BY_INSTANCE_LIMIT,
    DDS_REJECTED_BY_SAMPLES_LIMIT,
    DDS_REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT
} DDS_SampleRejectedStatusKind;

struct DDS_RequestedDeadlineMissedStatus {
DDS_long total_count;
DDS_long total_count_change;
DDS_InstanceHandle_t last_instance_handle;
};
typedef struct DDS_RequestedDeadlineMissedStatus DDS_RequestedDeadlineMissedStatus;

struct DDS_RequestedIncompatibleQosStatus {
	DDS_long nope;
};
typedef struct DDS_RequestedIncompatibleQosStatus DDS_RequestedIncompatibleQosStatus;

struct DDS_SampleRejectedStatus {
	DDS_long nope;
};
typedef struct DDS_SampleRejectedStatus DDS_SampleRejectedStatus;

struct DDS_LivelinessChangedStatus {
DDS_long alive_count;
DDS_long not_alive_count;
DDS_long alive_count_change;
DDS_long not_alive_count_change;
DDS_InstanceHandle_t last_publication_handle;
};
typedef struct DDS_LivelinessChangedStatus DDS_LivelinessChangedStatus;

struct DDS_SubscriptionMatchedStatus {
DDS_long total_count;
DDS_long total_count_change;
DDS_long current_count;
DDS_long current_count_change;
DDS_InstanceHandle_t last_publication_handle;
};
typedef struct DDS_SubscriptionMatchedStatus DDS_SubscriptionMatchedStatus;

struct DDS_SampleLostStatus {
DDS_long total_count;
DDS_long total_count_change;
};
typedef struct DDS_SampleLostStatus DDS_SampleLostStatus;


typedef void (*DDS_DataReaderListener_RequestedDeadlineMissedListener)(DDS_DataReader reader, DDS_RequestedDeadlineMissedStatus* status);
typedef void (*DDS_DataReaderListener_RequestedIncompatibleQosListener)(DDS_DataReader reader, DDS_RequestedIncompatibleQosStatus* status);
typedef void (*DDS_DataReaderListener_SampleRejectedListener)(DDS_DataReader reader, DDS_SampleRejectedStatus* status);
typedef void (*DDS_DataReaderListener_LivelinessChangedListener)(DDS_DataReader reader, DDS_LivelinessChangedStatus* status);
typedef void (*DDS_DataReaderListener_DataAvailableListener)(DDS_DataReader reader);
typedef void (*DDS_DataReaderListener_SubscriptionMatchListener)(DDS_DataReader reader, DDS_SubscriptionMatchedStatus* status);
typedef void (*DDS_DataReaderListener_SampleLostListener)(DDS_DataReader reader, DDS_SampleLostStatus* status);


struct DDS_DataReaderListener
{
void *listener_data;
DDS_DataReaderListener_RequestedDeadlineMissedListener on_requested_deadline_missed;
DDS_DataReaderListener_RequestedIncompatibleQosListener on_requested_incompatible_qos;
DDS_DataReaderListener_SampleRejectedListener on_sample_rejected;
DDS_DataReaderListener_LivelinessChangedListener on_liveliness_changed;
DDS_DataReaderListener_DataAvailableListener on_data_available;
DDS_DataReaderListener_SubscriptionMatchListener on_subscription_matched;
DDS_DataReaderListener_SampleLostListener on_sample_lost;
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

DDS_ReturnCode_t DDS_DataReader_set_listener (
		DDS_DataReader _this,
		const struct DDS_DataReaderListener* a_listener,
		const DDS_StatusMask mask);


#endif   /* ----- #ifndef DDS_DCPS_H_INC  ----- */
