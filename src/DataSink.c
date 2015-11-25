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
	DataReader_t *readers[SDDS_DATA_READER_MAX_OBJS];

#if defined(__GNUC__) && __GNUC_MINOR__ < 6
#pragma GCC diagnostic error "-Woverflow"
#endif
	unsigned int remaining_datareader :4;
};

static Msg_t *msg;
static DataSink_t dsStruct;
DataSink_t *dataSink = &dsStruct;

static SNPS_Address_t addr;

rc_t parseData(NetBuffRef_t *buff);
rc_t submitData(void);
rc_t checkDomain(NetBuffRef_t *buff, domainid_t domain);
rc_t checkTopic(NetBuffRef_t *buff, topicid_t topic);

rc_t BuiltinTopicDataReader_encode(byte_t* buff, Data data, size_t* size);

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t DataSink_getTopic(DDS_DCPSSubscription *st, topicid_t id, Topic_t **topic) {
	uint8_t index;
	for (index = 0; index < SDDS_DATA_READER_MAX_OBJS; index++) {
		if (dataSink->readers[index]
        &&  DataReader_topic (dataSink->readers[index])->id == id) {
			if (st != NULL) {
				st->key = DataReader_id (dataSink->readers[index]);
				st->participant_key = BuiltinTopic_participantID;
				st->topic_id = DataReader_topic (dataSink->readers[index])->id;
			}
			if (topic != NULL) {
				*topic = DataReader_topic (dataSink->readers[index]);
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
//	    case (SDDS_SNPS_T_TSSIMPLE) :
//		break;
//	    case (SDDS_SNPS_T_STATUS) :
//		break;
		default:
			// go to next submsg
			// unknown content

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
		DataReader_t *dr = dataSink->readers[index];
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

rc_t DataSink_init(void) {
#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Woverflow"
#endif

	dataSink->remaining_datareader = SDDS_DATA_READER_MAX_OBJS;

#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#endif

	return SDDS_RT_OK;
}
#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
DataReader_t * DataSink_create_datareader(Topic_t *topic, Qos qos, Listener listener, StatusMask sm)
{
	qos = qos;
	sm = sm;
	DataReader_t *dr = DataReader_new (topic, qos, listener, sm);
    dataSink->readers[SDDS_DATA_READER_MAX_OBJS - dataSink->remaining_datareader] = dr;
	dataSink->remaining_datareader--;

    Log_debug ("Create data reader with id %d at %p\n", DataReader_id (dr), (void *) dr);
    Log_debug ("Remaining data reader %d/%d.\n", SDDS_DATA_READER_MAX_OBJS, dataSink->remaining_datareader);
	return dr;
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
	Topic_t *topic;
	// check if there is a topic provided (should be)
	// data without topic are useless for this version!

	if (buff->curTopic == NULL) {
		// should not happen

		SNPS_discardSubMsg(buff);

		return SDDS_RT_FAIL;
	} else {

		topic = buff->curTopic;
	}
	// get msg from topic

	if (Topic_getFreeMsg(topic, &msg) != SDDS_RT_OK) {

		SNPS_discardSubMsg(buff);

		return SDDS_RT_FAIL;
	}
	//msg->isEmpty = false;
	// parse data
	Data newData;

	Msg_getData(msg, &newData);

	rc_t ret = SNPS_readData(buff, topic->Data_decode, newData);

	return ret;
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
		SNPS_writeData(buf, BuiltinTopicDataReader_encode,
				(Data) dataSink->readers[index]); }

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
