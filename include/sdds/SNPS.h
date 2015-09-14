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

#include <netdb.h>

typedef uint8_t subMsg_t;
typedef uint8_t addrType_t;
typedef uint8_t castType_t;

#define SDDS_SNPS_CAST_UNICAST 		0
#define SDDS_SNPS_CAST_MULTICAST	1
#define SDDS_SNPS_CAST_BROADCAST	2

#define SDDS_SNPS_ADDR_IPV4		0
#define SDDS_SNPS_ADDR_IPV6		1

#define SDDS_SNPS_T_DOMAIN		 0
#define SDDS_SNPS_T_TOPIC		 1
#define SDDS_SNPS_T_DATA			 2
#define SDDS_SNPS_T_SEQNR		 3
#define SDDS_SNPS_T_TSSIMPLE		 4
#define SDDS_SDDS_SNPS_T_ACKSEQ		 5
#define SDDS_SDDS_SNPS_T_NACKSEQ		 6
#define SDDS_SNPS_T_STATUS		 7
#define SDDS_SNPS_T_ADDRESS		8

#define SDDS_SNPS_T_ACK			17
#define SDDS_SNPS_T_NACK			18
#define SDDS_SNPS_T_SEP			19
#define SDDS_SNPS_T_TSDDS		20
#define SDDS_SNPS_T_TSUSEC		33
#define SDDS_SNPS_T_TSMSEC		34
#define SDDS_SNPS_T_SEQNRBIG		35
#define SDDS_SNPS_T_SEQNRSMALL	36
#define SDDS_SNPS_T_SEQNRHUGE	37

#define SDDS_SNPS_T_UNKNOWN		0xff

// type id for the submessage field
#define SDDS_SNPS_SUBMSG_DOMAIN		0x00
#define SDDS_SNPS_SUBMSG_TOPIC		0x01
#define SDDS_SNPS_SUBMSG_DATA		0x02
#define SDDS_SNPS_SUBMSG_SEQNR		0x03
#define SDDS_SNPS_SUBMSG_TS			0x04
#define SDDS_SNPS_SUBMSG_ACKSEQ		0x05
#define SDDS_SNPS_SUBMSG_NACKSEQ	0x06
#define SDDS_SNPS_SUBMSG_STATUS		0x07
#define SDDS_SNPS_SUBMGS_ADDR		0x08

#define SDDS_SNPS_SUBMSG_EXTENDED	0x0f

#define SDDS_SNPS_EXTSUBMSG_ACK		0x00
#define SDDS_SNPS_EXTSUBMSG_NACK	0x01
#define SDDS_SNPS_EXTSUBMSG_SEP		0x02
#define SDDS_SNPS_EXTSUBMSG_TSDDS	0x03
#define SDDS_SNPS_EXTSUBMSG_ADDR16	0x04
#define SDDS_SNPS_EXTSUBMSG_CRC		0x05
#define SDDS_SNPS_EXTSUBMSG_TOPIC	0x06

#define SDDS_SNPS_EXTSUBMSG_EXTENDED		0x0f

#define SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_CHAR	20		// ffFS::00GG:GGGG:GGGG
#define SNPS_MULTICAST_COMPRESSION_MIN_LENGTH_IN_CHAR	7		// ffFS::G
#define SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_BYTE	6		// 48 Bit Compression

#define SDDS_SNPS_ADDR_STR_LENGTH	SNPS_MULTICAST_COMPRESSION_MAX_LENGTH_IN_CHAR + 1	//NI_MAXHOST

struct SNPS_Address {
	char addr[SDDS_SNPS_ADDR_STR_LENGTH];
	castType_t addrCast;
	addrType_t addrType;
};
typedef struct SNPS_Address SNPS_Address_t;

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

rc_t SNPS_writeAddress(NetBuffRef ref, castType_t castType, addrType_t addrType, uint8_t *addr, uint8_t addrLen);
rc_t SNPS_readAddress(NetBuffRef ref, castType_t *addrCast, addrType_t *addrType, char *addr);

rc_t SNPS_readHeader(NetBuffRef ref);
rc_t SNPS_readDomain(NetBuffRef ref, domainid_t* domain);
rc_t SNPS_readTopic(NetBuffRef ref, topicid_t* topic);
rc_t SNPS_readData(NetBuffRef ref, rc_t (*TopicMarshalling_decode)(byte_t* buff, Data data, size_t* size), Data data);
#endif   /* ----- #ifndef SNPS_H_INC  ----- */
