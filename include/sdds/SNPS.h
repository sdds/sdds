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
#include "os-ssal/TimeMng.h"
#include "NetBuffRef.h"
#include "TopicMarshalling.h"
#include "Qos.h"

typedef uint8_t subMsg_t;
typedef uint8_t addrType_t;
typedef uint8_t castType_t;

#define SDDS_SNPS_CAST_UNICAST 	0
#define SDDS_SNPS_CAST_MULTICAST	1
#define SDDS_SNPS_CAST_BROADCAST	2

#define SDDS_SNPS_ADDR_IPV4		0
#define SDDS_SNPS_ADDR_IPV6		1

// unique ids of (extended)subMsg-type
#define SDDS_SNPS_T_DOMAIN		   0
#define SDDS_SNPS_T_TOPIC		   1
#define SDDS_SNPS_T_DATA			2
#define SDDS_SNPS_T_SEQNR		   3
#define SDDS_SNPS_T_TSSIMPLE		4
#define SDDS_SNPS_T_ACKSEQ	      5
#define SDDS_SNPS_T_NACKSEQ	   6
#define SDDS_SNPS_T_STATUS		   7
#define SDDS_SNPS_T_ADDRESS		8

#define SDDS_SNPS_T_ACK			   17
#define SDDS_SNPS_T_NACK		   18
#define SDDS_SNPS_T_SEP			   19
#define SDDS_SNPS_T_TSDDS		   20
#define SDDS_SNPS_T_ADDR16		   21
#define SDDS_SNPS_T_CRC 		   22
#define SDDS_SNPS_T_TSUSEC		   33
#define SDDS_SNPS_T_TSMSEC		   34
#define SDDS_SNPS_T_SEQNRSMALL	35
#define SDDS_SNPS_T_SEQNRBIG		36
#define SDDS_SNPS_T_SEQNRHUGE	   37
#define SDDS_SNPS_T_FRAG   	   38
#define SDDS_SNPS_T_FRAGNACK	   39
#define SDDS_SNPS_T_UNKNOWN		0xff

// 4-bit id for distinguishing subMsg-type
#define SDDS_SNPS_SUBMSG_DOMAIN		   0x00
#define SDDS_SNPS_SUBMSG_TOPIC		   0x01
#define SDDS_SNPS_SUBMSG_DATA		      0x02
#define SDDS_SNPS_SUBMSG_SEQNR		   0x03
#define SDDS_SNPS_SUBMSG_TS			   0x04
#define SDDS_SNPS_SUBMSG_ACKSEQ		   0x05
#define SDDS_SNPS_SUBMSG_NACKSEQ	      0x06
#define SDDS_SNPS_SUBMSG_STATUS		   0x07
#define SDDS_SNPS_SUBMGS_ADDR		      0x08
#define SDDS_SNPS_SUBMSG_EXTENDED	   0x0f

#define SDDS_SNPS_EXTSUBMSG_ACK		   0x00
#define SDDS_SNPS_EXTSUBMSG_NACK	      0x01
#define SDDS_SNPS_EXTSUBMSG_SEP		   0x02
#define SDDS_SNPS_EXTSUBMSG_TSDDS	   0x03
#define SDDS_SNPS_EXTSUBMSG_ADDR16	   0x04
#define SDDS_SNPS_EXTSUBMSG_CRC		   0x05
#define SDDS_SNPS_EXTSUBMSG_TSUSEC     0x06
#define SDDS_SNPS_EXTSUBMSG_TSMSEC     0x07
#define SDDS_SNPS_EXTSUBMSG_SEQNRSMALL 0x08
#define SDDS_SNPS_EXTSUBMSG_SEQNRBIG   0x09
#define SDDS_SNPS_EXTSUBMSG_SEQNRHUGE  0x0A
#define SDDS_SNPS_EXTSUBMSG_TOPIC	   0x0B
#define SDDS_SNPS_EXTSUBMSG_FRAG       0x0C
#define SDDS_SNPS_EXTSUBMSG_FRAGNACK   0x0D
#define SDDS_SNPS_EXTSUBMSG_EXTENDED	0x0f

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

rc_t
SNPS_evalSubMsg (NetBuffRef_t *ref, subMsg_t* type);

rc_t
SNPS_discardSubMsg (NetBuffRef_t *ref);

// go to next occurence of the sub msg type provided, but not parse the submsg!
rc_t SNPS_gotoNextSubMsg(NetBuffRef_t *buff, subMsg_t type);

rc_t SNPS_initFrame(NetBuffRef_t *ref);
rc_t SNPS_updateHeader(NetBuffRef_t *ref);
rc_t SNPS_writeDomain(NetBuffRef_t *ref, domainid_t domain);
rc_t SNPS_writeTopic(NetBuffRef_t *ref, topicid_t topic);
rc_t SNPS_writeData(NetBuffRef_t *ref, TopicMarshalling_encode_fn encode_fn, Data);
rc_t SNPS_writeTSsimple(NetBuffRef_t *ref, TimeStampSimple_t* ts);
rc_t SNPS_writeStatus(NetBuffRef_t *ref);
#if defined SDDS_QOS_RELIABILITY
rc_t SNPS_writeSeqNr (NetBuffRef_t* ref, seqNr_t seqNr);
rc_t SNPS_writeSeqNrSmall (NetBuffRef_t* ref, seqNr_t seqNr);
rc_t SNPS_writeSeqNrBig (NetBuffRef_t* ref, seqNr_t seqNr);
rc_t SNPS_writeSeqNrHUGE (NetBuffRef_t* ref, seqNr_t seqNr);
#endif
rc_t SNPS_writeAckSeq(NetBuffRef_t *ref);
rc_t SNPS_writeNackSeq(NetBuffRef_t *ref);
rc_t SNPS_writeNack(NetBuffRef_t *ref);
rc_t SNPS_writeAck(NetBuffRef_t *ref);
rc_t SNPS_writeTSuSec(NetBuffRef_t *ref);
rc_t SNPS_writeTSmSec(NetBuffRef_t *ref);
rc_t SNPS_writeTSDDS(NetBuffRef_t *ref);
rc_t SNPS_writeSep(NetBuffRef_t *ref);
rc_t SNPS_writeAddress(NetBuffRef_t *ref, castType_t castType, addrType_t addrType, uint8_t *addr, uint8_t addrLen);

rc_t SNPS_readAddress(NetBuffRef_t *ref, castType_t *addrCast, addrType_t *addrType, char *addr);
rc_t SNPS_readHeader(NetBuffRef_t *ref);
rc_t SNPS_readDomain(NetBuffRef_t *ref, domainid_t* domain);
rc_t SNPS_readTopic(NetBuffRef_t *ref, topicid_t* topic);
#if defined SDDS_QOS_RELIABILITY
rc_t SNPS_readSeqNr (NetBuffRef_t* ref, seqNr_t* seqNr);
rc_t SNPS_readSeqNrSmall (NetBuffRef_t* ref, seqNr_t* seqNr);
rc_t SNPS_readSeqNrBig (NetBuffRef_t* ref, seqNr_t* seqNr);
rc_t SNPS_readSeqNrHUGE (NetBuffRef_t* ref, seqNr_t* seqNr);
#endif
rc_t SNPS_readData (NetBuffRef_t *ref, TopicMarshalling_decode_fn decode_fn, Data data);

rc_t SNPS_IPv6_str2Addr(char *charAddr, uint8_t *byteAddr, uint8_t *addrLen);
rc_t SNPS_IPv6_addr2Str(uint8_t *byteAddr, char *charAddr);
#endif   /* ----- #ifndef SNPS_H_INC  ----- */
