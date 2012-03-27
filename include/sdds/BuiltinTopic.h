/*
 * BuiltinTopic.h
 *
 *  Created on: Jul 19, 2010
 *      Author: kai
 */

#ifndef BUILTINTOPIC_H_
#define BUILTINTOPIC_H_

#include "sdds_types.h"
#include "Locator.h"
#include "dds/DDS_DCPS.h"
#include "NetBuffRef.h"

// should be defined in the plattform specific network impl
extern Locator builtinTopicNetAddress;

/**
 * Data for the builtintopics:
 *
 * There should be information about the qos as well
 *
 * DomainParticipant:
 * 	Non
 *
 * Topic:
 * 	DomainID (size?)
 * 	TopicID  (size?)
 *
 * DataWriter:
 * 	DomainID
 * 	TopicID
 *
 * DataReader:
 * 	DomainID
 * 	TopicID
 *
 */


// IF APP IFACE + BUILTINTOPIC?

typedef  uint8_t BuiltinTopicKey_t;

struct DDS_DCPSParticipant_t {
    BuiltinTopicKey_t key;
    // IF BUILTINTOPIC AND QOS AND USERDATA
    // DDS_UserDataQosPolicy user_data;
    // ENDIF
};
typedef struct DDS_DCPSParticipant_t DDS_DCPSParticipant;
/*
DDS_ReturnCode_t DDS_DCPSParticipantDataReader_take_next_sample (
	    DDS_DataReader _this,
	    DDS_DCPSParticipant** data_values,
	    DDS_SampleInfo *sample_info);
*/
struct DDS_DCPSTopic_t {
    BuiltinTopicKey_t key;
    DDS_string name;
    DDS_string type_name;
    // IF QOS AND Foo
    // TODO
    // ENDIF
};
typedef struct DDS_DCPSTopic_t DDS_DCPSTopic;
/*
DDS_ReturnCode_t DDS_DCPSTopicDataReader_take_next_sample (
	    DDS_DataReader _this,
	    DDS_DCPSTopic** data_values,
	    DDS_SampleInfo *sample_info);
*/
struct DDS_DCPSPublication_t {
    BuiltinTopicKey_t key;
    BuiltinTopicKey_t participant_key;
    DDS_string topic_name;
    DDS_string type_name;
    // IF QOS AND FOO
    // TODO
    // ENDIF
};
typedef struct DDS_DCPSPublication_t  DDS_DCPSPublication;

/*
DDS_ReturnCode_t DDS_DCPSPublicationDataReader_take_next_sample (
	    DDS_DataReader _this,
	    DDS_DCPSPublication** data_values,
	    DDS_SampleInfo *sample_info);
*/

struct DDS_DCPSSubscription_t {
    BuiltinTopicKey_t key;
    BuiltinTopicKey_t participant_key;
    DDS_string topic_name;
    DDS_string type_name;
    // IF QOS AND FOO
    // TODO
    // ENDIF
};
typedef struct DDS_DCPSSubscription_t DDS_DCPSSubscription;

/*
DDS_ReturnCode_t DDS_DCPSSubscriptionDataReader_take_next_sample (
	    DDS_DataReader _this,
	    DDS_DCPSSubscription** data_values,
	    DDS_SampleInfo *sample_info);
*/

// ENDIF APP + BUILTINTOPIC


#define SDDS_BUILTINTOPIC_PARTICIPANT_TOPIC_ID 	0x01
#define SDDS_BUILTINTOPIC_TOPIC_TOPIC_ID	0x02
#define SDDS_BUILTINTOPIC_PUBLICATION_TOPIC_ID	0x03
#define SDDS_BUILTINTOPIC_SUBSCRIPTION_TOPIC_ID	0x04




rc_t BuiltinTopic_init(void);

rc_t BuiltinTopic_publishAll(void);

rc_t BuiltinTopic_publishDP(void);

rc_t BuiltinTopic_publishPub(void);

rc_t BuiltinTopic_publishSub(void);

rc_t BuiltinTopic_writeDomainParticipant2Buf(NetBuffRef buf);
rc_t BuiltinTopic_writeTopics2Buf(NetBuffRef buf);
rc_t BuiltinTopic_writeDataWriters2Buf(NetBuffRef buf);
rc_t BuiltinTopic_writeDataReaders2Buf(NetBuffRef buf);


#endif /* BUILTINTOPIC_H_ */
