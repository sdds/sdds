/*
 * =====================================================================================
 *
 *       Filename:  DataSink.c
 *
 *    Description:  Impl file for the DataSink class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 08:57:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "DataSink.h"

struct _DataSink_t {
	DataReader_t readers[SDDS_DATA_READER_MAX_OBJS];
    uint64_t allocated_readers;
};
static DataSink_t _dataSink;
static DataSink_t *self = &_dataSink;

static Msg_t *msg;
static SNPS_Address_t addr;

rc_t parseData(NetBuffRef_t *buff);
rc_t submitData(void);
rc_t checkDomain(NetBuffRef_t *buff, domainid_t domain);
rc_t checkTopic(NetBuffRef_t *buff, topicid_t topic);
rc_t BuiltinTopicDataReader_encode(byte_t* buff, Data data, size_t* size);


//  ---------------------------------------------------------------------------
//  Initialize this class

rc_t
DataSink_init (void) {
	return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t
DataSink_getTopic (DDS_DCPSSubscription *st, topicid_t id, Topic_t **topic) {
	uint8_t index;
	for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
		if (BitArray_check (&self->allocated_readers, index)
        &&  DataReader_topic (&self->readers[index])->id == id) {
			if (st != NULL) {
				st->key = DataReader_id (&self->readers[index]);
				st->participant_key = BuiltinTopic_participantID;
				st->topic_id = DataReader_topic (&self->readers[index])->id;
			}
			if (topic != NULL) {
				*topic = DataReader_topic (&self->readers[index]);
			}
			return SDDS_RT_OK;
		}
	}
	return SDDS_RT_FAIL;
}
#endif

rc_t DataSink_getAddr(SNPS_Address_t *address) {
	address->addrType = addr.addrType;
	address->addrCast = addr.addrCast;
	memcpy(address->addr, addr.addr, SDDS_SNPS_ADDR_STR_LENGTH);
	return SDDS_RT_OK;
}

static void getAddress(NetBuffRef_t *buff) {
	memset(addr.addr, 0, SDDS_SNPS_ADDR_STR_LENGTH);
	addr.addrType = 0;
	addr.addrCast = 0;
	SNPS_readAddress(buff, &addr.addrCast, &addr.addrType, addr.addr);
}

rc_t DataSink_processFrame(NetBuffRef_t *buff) {
	rc_t ret;

	if (buff == NULL)
		return SDDS_RT_FAIL;

	// process the frame
	// parse the header

	ret = SNPS_readHeader(buff);

	if (ret != SDDS_RT_OK) {
		// done reset the buffer

		NetBuffRef_renew(buff);

		return SDDS_RT_FAIL;
	}

	// should be NULL!
	msg = NULL;
	topicid_t topic;

	while (buff->subMsgCount > 0) {
		// get sum msg id
		subMsg_t type;

		SNPS_evalSubMsg(buff, &type);

		switch (type) {
		case (SDDS_SNPS_T_DOMAIN):
			// check data

			submitData();

			domainid_t domain;

			SNPS_readDomain(buff, &domain);

			// check domain
			checkDomain(buff, domain);

			break;
		case (SDDS_SNPS_T_TOPIC):
			// check data

			submitData();

			SNPS_readTopic(buff, &topic);

			// check topic
			checkTopic(buff, topic);

			break;
		case (SDDS_SNPS_T_DATA):
#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
			// check data

			if (DataSink_getTopic(NULL, topic, NULL) == SDDS_RT_OK) {
				submitData();

				ret = parseData(buff);

				if (ret != SDDS_RT_OK)
					return SDDS_RT_FAIL;
			}
			else {
                Log_debug ("Discard submessage\n");
				SNPS_discardSubMsg(buff);
			}
#endif
			break;
		case (SDDS_SNPS_T_ADDRESS):

			getAddress(buff);

			break;
        case (SDDS_SNPS_T_TSSIMPLE) :
        case (SDDS_SNPS_T_STATUS) :
		default:
			// go to next submsg
			// unknown content

            Log_debug ("Discard submessage\n");
			SNPS_discardSubMsg(buff);

			break;
		}

	}
	// submit the last decoded msg

	submitData();

	// done reset the buffer
	NetBuffRef_renew(buff);

	// ggf send events to the applications
#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
    uint8_t index;
	for (index = 0;	index < SDDS_DATA_READER_MAX_OBJS; index++) {
		DataReader_t *dr = &self->readers[index];
        if (!dr)
            continue;

		int tpc = DataReader_topic (dr)->id;
		if ((topic == tpc) && DataReader_on_data_avail_listener (dr)) {
            On_Data_Avail_Listener on_data_avail_listener = DataReader_on_data_avail_listener (dr);
            on_data_avail_listener (dr);
		}
	}
#endif
	return SDDS_RT_OK;
}


#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
//  ---------------------------------------------------------------------------
//  Creates a new DataReader object

DataReader_t *
DataSink_create_datareader (Topic_t *topic, Qos qos, Listener listener, StatusMask sm)
{
	qos = qos;
	sm = sm;

    DataReader_t *reader = NULL;
    uint8_t index;
    for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
        //  Check if object at index has been allocated
        if (!BitArray_check (&self->allocated_readers, index)) {
            //  Allocate object at index
            BitArray_set (&self->allocated_readers, index);
            reader = &self->readers[index];
            // Initialize object properties
            reader->id = index;
            reader->topic = topic;
            reader->on_data_avail_listener = NULL;
        }
    }
    Log_debug ("Create data reader with id %d\n", DataReader_id (reader));
	return reader;
}
#endif


rc_t submitData(void) {
	if (msg != NULL) {
		// add msg to topic incomming queue
		// msg->dir = SDDS_MSG_DIR_INCOMMING;
		// msg->isRead = false;
		// msg->isNew = true;
		// TODO
		// impl of app  callback
		msg = NULL;
	}
	return SDDS_RT_OK;
}

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t parseData(NetBuffRef_t *buff) {
    assert (buff);
    assert (buff->curTopic);

	Topic_t *topic = buff->curTopic;
	//  Get msg from topic
	if (Topic_getFreeMsg (topic, &msg) != SDDS_RT_OK) {
		SNPS_discardSubMsg (buff);
		return SDDS_RT_FAIL;
	}
	//  Parse data
	Data newData;
	Msg_getData (msg, &newData);
	return SNPS_readData (buff, topic->Data_decode, newData);
}
#endif

rc_t checkDomain(NetBuffRef_t *buff, domainid_t domain) {
	if (TopicDB_checkDomain(domain) == false) {
		SNPS_gotoNextSubMsg(buff, SDDS_SNPS_T_DOMAIN);
	} else {
		buff->curDomain = domain;
	}

	return SDDS_RT_OK;
}

rc_t checkTopic(NetBuffRef_t *buff, topicid_t topic) {
	Topic_t *t_ptr = TopicDB_getTopic(topic);
	if (t_ptr == NULL) {
		SNPS_gotoNextSubMsg(buff, SDDS_SNPS_T_TOPIC);
	} else {
		buff->curTopic = t_ptr;
	}

	return SDDS_RT_OK;
}

rc_t BuiltinTopic_writeDataReaders2Buf(NetBuffRef_t *buf) {
	SNPS_writeTopic(buf, DDS_DCPS_SUBSCRIPTION_TOPIC);
    uint8_t index;
	for (index = 0;	index < SDDS_DATA_READER_MAX_OBJS; index++) {
		SNPS_writeData (buf, BuiltinTopicDataReader_encode,
				(Data) &self->readers[index]);
    }

	return SDDS_RT_OK;
}
rc_t BuiltinTopicDataReader_encode(byte_t* buff, Data data, size_t* size) {
#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
	DataReader_t *dr = (DataReader_t *) data;
	*size = 0;
	Marshalling_enc_uint8(buff + (*size), &(DataReader_topic (dr)->domain));
	*size += sizeof(domainid_t);
	Marshalling_enc_uint8(buff + (*size), &(DataReader_topic (dr)->id));
	*size += sizeof(topicid_t);
#endif

	return SDDS_RT_OK;
}
