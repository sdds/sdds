/*
 * =====================================================================================
 *
 *       Filename:  SNPS.h
 *
 *    Description:  SNPS protocoll class, does the en- and decoding of the 
 *    SNPS-protocoll. 
 *
 *        Version:  1.0
 *        Created:  24.02.2010 13:02:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  SNPS_H_INC
#define  SNPS_H_INC

#include "sdds_types.h"
#include "TimeMng.h"
#include "NetBuffRef.h"

typedef uint8_t subMsg_t;

#define SNPS_T_DOMAIN		 0
#define SNPS_T_TOPIC		 1
#define SNPS_T_DATA		 2
#define SNPS_T_SEQNR		 3
#define SNPS_T_TSSIMPLE		 4
#define SNPS_T_ACKSEQ		 5
#define SNPS_T_NACKSEQ		 6
#define SNPS_T_STATUS		 7

#define SNPS_T_ACK		17
#define SNPS_T_NACK		18
#define SNPS_T_SEP		19
#define SNPS_T_TSDDS		20
#define SNPS_T_TSUSEC		33
#define SNPS_T_TSMSEC		34
#define SNPS_T_SEQNRBIG		35
#define SNPS_T_SEQNRSMALL	36
#define SNPS_T_SEQNRHUGE	37
#define SNPS_T_ADDRESS		38

#define SNPS_T_UNKNOWN		0xff

// type id for the submessage field
#define SNPS_SUBMSG_DOMAIN		0x00
#define SNPS_SUBMSG_TOPIC		0x01
#define SNPS_SUBMSG_DATA		0x02
#define SNPS_SUBMSG_SEQNR		0x03
#define SNPS_SUBMSG_TS			0x04
#define SNPS_SUBMSG_ACKSEQ		0x05
#define SNPS_SUBMSG_NACKSEQ		0x06
#define SNPS_SUBMSG_STATUS		0x07

#define SNPS_SUBMSG_EXTENDED		0x0f

#define SNPS_EXTSUBMSG_ACK		0x00
#define SNPS_EXTSUBMSG_NACK		0x01
#define SNPS_EXTSUBMSG_SEP		0x02
#define SNPS_EXTSUBMSG_TSDDS		0x03
#define SNPS_EXTSUBMSG_ADDR16		0x04

#define SNPS_EXTSUBMSG_EXTENDED		0x0f


rc_t SNPS_evalSubMsg(NetBuffRef ref, subMsg_t* type);
rc_t SNPS_discardSubMsg(NetBuffRef ref);
// go to next occurence of the sub msg type provided, but not parse the submsg!
rc_t SNPS_gotoNextSubMsg(NetBuffRef buff, subMsg_t type);

rc_t SNPS_initFrame(NetBuffRef ref);
rc_t SNPS_updateHeader(NetBuffRef ref);
rc_t SNPS_writeDomain(NetBuffRef ref, domainid_t domain);
rc_t SNPS_writeTopic(NetBuffRef ref, topicid_t topic);
rc_t SNPS_writeData(NetBuffRef ref, rc_t (*TopicMarshalling_encode)(byte_t* buff, Data data, size_t* size), Data);
rc_t SNPS_writeTSsimple(NetBuffRef ref, TimeStampSimple_t* ts);
rc_t SNPS_writeStatus(NetBuffRef ref);
rc_t SNPS_writeSeqNr(NetBuffRef ref);
rc_t SNPS_writeAckSeq(NetBuffRef ref);
rc_t SNPS_writeNackSeq(NetBuffRef ref);
rc_t SNPS_writeNack(NetBuffRef ref);
rc_t SNPS_writeAck(NetBuffRef ref);
rc_t SNPS_writeTSuSec(NetBuffRef ref);
rc_t SNPS_writeTSmSec(NetBuffRef ref);
rc_t SNPS_writeSeqNrBig(NetBuffRef ref);
rc_t SNPS_writeSeqNrSmall(NetBuffRef ref);
rc_t SNPS_writeSeqNrHUGE(NetBuffRef ref);
rc_t SNPS_writeTSDDS(NetBuffRef ref);
rc_t SNPS_writeSep(NetBuffRef ref);
rc_t SNPS_writeAddress(NetBuffRef ref);


rc_t SNPS_readHeader(NetBuffRef ref);
rc_t SNPS_readDomain(NetBuffRef ref, domainid_t* domain);
rc_t SNPS_readTopic(NetBuffRef ref, topicid_t* topic);
rc_t SNPS_readData(NetBuffRef ref, rc_t (*TopicMarshalling_decode)(byte_t* buff, Data data, size_t* size), Data data);
#endif   /* ----- #ifndef SNPS_H_INC  ----- */
