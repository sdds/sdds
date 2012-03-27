/*
 * =====================================================================================
 *
 *       Filename:  SNPS.c
 *
 *    Description:  SNPS protocoll class impl file
 *
 *        Version:  1.0
 *        Created:  27.02.2010 15:13:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "SNPS.h"
#include "Marshalling.h"


#define START (ref->buff_start + ref->curPos)

rc_t SNPS_evalSubMsg(NetBuffRef ref, subMsg_t* type)
{

    uint8_t read;
    Marshalling_dec_uint8(START, &read);

    // it is a basic submessage: sub msg id == sub msg id
    if ((read & 0x0f) < 16){
	*type = (read & 0x0f);
	return SDDS_RT_OK;
    } 

    // it is an extended submessage
    // eval the next 4 bits
    switch ((read >> 4) & 0x0f){
	case (SNPS_EXTSUBMSG_ACK):
	    *type = SNPS_T_ACK;
	    break;
	case (SNPS_EXTSUBMSG_NACK):
	    *type = SNPS_T_NACK;
	    break;
	case (SNPS_EXTSUBMSG_SEP):
	    *type = SNPS_T_SEP;
	    break;
	case (SNPS_EXTSUBMSG_TSDDS):
	    *type = SNPS_T_TSDDS;
	    break;
	case (SNPS_EXTSUBMSG_EXTENDED):
	    // read the next byte etc
	    // TODO
	    *type = SNPS_T_UNKNOWN;
	    break;
	default:
	    *type = SNPS_T_UNKNOWN;
	    break;
    }
   

    return SDDS_RT_OK;
}

rc_t SNPS_discardSubMsg(NetBuffRef ref)
{
    uint8_t read;
    Marshalling_dec_uint8(START, &read);
   
    // submsg will be removed, all the same
    // read at least the basic submsgheader
    ref->subMsgCount--;
    ref->curPos += 1;

    switch ((read & 0x0f)){
	// data field. pos counter + lenght and submsg
	case (SNPS_SUBMSG_DATA):
	    ref->curPos += ((read >> 4) & 0x0f);
	    return SDDS_RT_OK;
	    // simple timestamp pos counter + 2 byte and submsg
	case (SNPS_SUBMSG_TS):
	    ref->curPos += 2;
	    return SDDS_RT_OK;

	    // extended submsg eval later
	case (SNPS_SUBMSG_EXTENDED):
	    break;

	    // normale submsg with the size of 8 bit
	default: 
	    return SDDS_RT_OK;
    }



    // it is an extended submessage
    // eval the next 4 bits
    switch ((read >> 4) & 0x0f){
	// 1 byte extsubmsg
	case (SNPS_EXTSUBMSG_ACK):
	case (SNPS_EXTSUBMSG_NACK):
	case (SNPS_EXTSUBMSG_SEP):
	    break;
	case (SNPS_EXTSUBMSG_TSDDS):
	    // 1 byte header + 2 x 4 byte sec and nanosec
	    ref->curPos += 8;
	    break;
	case (SNPS_EXTSUBMSG_EXTENDED):
	    // TODO
	    return SDDS_RT_FAIL;
	    break;
	default:
	    // shouldn't happen
	    return SDDS_RT_FAIL;
    }


    return SDDS_RT_OK;
}

rc_t SNPS_gotoNextSubMsg(NetBuffRef buff, subMsg_t type)
{
    subMsg_t readType;
    while (buff->subMsgCount > 0){
	SNPS_evalSubMsg(buff, &readType);
	if (readType != type){
	    if (SNPS_discardSubMsg(buff) != SDDS_RT_OK){
		return SDDS_RT_FAIL;
	    }
	} else {
	    return SDDS_RT_OK;
	}
    }
    return SDDS_RT_OK;
}

rc_t SNPS_initFrame(NetBuffRef ref)
{
    // write header
    version_t v = sDDS_NET_VERSION;
    Marshalling_enc_uint8(ref->buff_start, &v);
    ref->curPos += sizeof(version_t);
    // place holder
    Marshalling_enc_uint8( START, &(ref->subMsgCount));
    ref->curPos += sizeof(uint8_t);

    return SDDS_RT_OK;
}
rc_t SNPS_readHeader(NetBuffRef ref)
{
    version_t v;
    Marshalling_dec_uint8(ref->buff_start, &v);
    ref->curPos += sizeof(version_t);
    if (v != sDDS_NET_VERSION){
	return SDDS_RT_FAIL;
    }
    Marshalling_dec_uint8(START, &(ref->subMsgCount));
    ref->curPos += sizeof(uint8_t);

    return SDDS_RT_OK;
}

rc_t SNPS_updateHeader(NetBuffRef ref)
{
    Marshalling_enc_uint8( (ref->buff_start + sizeof(version_t)), &(ref->subMsgCount));

    return SDDS_RT_OK;
}

rc_t SNPS_writeDomain(NetBuffRef ref, domainid_t domain)
{
    Marshalling_enc_SubMsg(START, SNPS_SUBMSG_DOMAIN, (uint8_t)domain);
    ref->curPos += 1;
    ref->subMsgCount +=1;

    return SDDS_RT_OK;
}
rc_t SNPS_readDomain(NetBuffRef ref, domainid_t* domain)
{
    Marshalling_dec_SubMsg(START, SNPS_SUBMSG_DOMAIN, (uint8_t*)domain);
    ref->curPos += 1;
    ref->subMsgCount -=1;

    return SDDS_RT_OK;
}

rc_t SNPS_writeTopic(NetBuffRef ref, topicid_t topic)
{
    Marshalling_enc_SubMsg(START, SNPS_SUBMSG_TOPIC, (uint8_t)topic);
    ref->curPos +=1;
    ref->subMsgCount +=1;

    return SDDS_RT_OK;
}
rc_t SNPS_readTopic(NetBuffRef ref, topicid_t* topic)
{
    Marshalling_dec_SubMsg(START, SNPS_SUBMSG_TOPIC, (uint8_t*)topic);
    ref->curPos +=1;
    ref->subMsgCount -=1;

    return SDDS_RT_OK;
}

rc_t SNPS_writeData(NetBuffRef ref, rc_t (*TopicMarshalling_encode)(byte_t* buff, Data data, size_t* size), Data d)
{
    size_t writtenBytes = 0;

    // start 1 byte later, the header have to be written if the size is known
    byte_t* s = (ref->buff_start + ref->curPos + 1);


    if ((*TopicMarshalling_encode)(s, d, &writtenBytes) != SDDS_RT_OK)
	return SDDS_RT_FAIL;

    Marshalling_enc_SubMsg(START, SNPS_SUBMSG_DATA, (uint8_t) writtenBytes);

    // data header
    ref->curPos += 1;
    // data itself
    ref->curPos += writtenBytes;
    ref->subMsgCount +=1;

    return SDDS_RT_OK;
}

rc_t SNPS_readData(NetBuffRef ref, rc_t (*TopicMarshalling_decode)(byte_t* buff, Data data, size_t* size), Data data)
{

    size_t size = 0;
    Marshalling_dec_SubMsg(START, SNPS_SUBMSG_DATA, (uint8_t*) &size);
    ref->curPos += 1;


    byte_t* s = (ref->buff_start + ref->curPos);

    if ((*TopicMarshalling_decode)(s, data, &size) != SDDS_RT_OK){
	return SDDS_RT_FAIL;
    }
    ref->curPos += size;
    ref->subMsgCount -= 1;

    return SDDS_RT_OK;
}
/*
rc_t SNPS_writeTSsimple(NetBuffRef ref, TimeStampSimple_t* ts)
{
    Marshalling_enc_SubMsg(START, SNPS_SUBMSG_TS, ts->firstField);
    ref->curPos += 1;
    Marshalling_enc_uint8(START, &(ts->secondField));
    ref->curPos += 1;
    Marshalling_enc_uint8(START, &(ts->thirdField));
    ref->curPos += 1;
    
    ref->subMsgCount +=1;

    return SDDS_RT_OK;

}
*/
//rc_t SNPS_writeStatus(NetBuffRef ref);
//rc_t SNPS_writeSeqNr(NetBuffRef ref);
//rc_t SNPS_writeAckSeq(NetBuffRef ref);
//rc_t SNPS_writeNackSeq(NetBuffRef ref);
//rc_t SNPS_writeNack(NetBuffRef ref);
//rc_t SNPS_writeAck(NetBuffRef ref);
//rc_t SNPS_writeTSuSec(NetBuffRef ref);
//rc_t SNPS_writeTSmSec(NetBuffRef ref);
//rc_t SNPS_writeSeqNrBig(NetBuffRef ref);
//rc_t SNPS_writeSeqNrSmall(NetBuffRef ref);
//rc_t SNPS_writeSeqNrHUGE(NetBuffRef ref);
//rc_t SNPS_writeTSDDS(NetBuffRef ref);
//rc_t SNPS_wrieSep(NetBuffRef ref);
//rc_t SNPS_writeAddress(NetBuffRef ref);

