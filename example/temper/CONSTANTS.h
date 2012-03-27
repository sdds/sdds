/*
 * =====================================================================================
 *
 *       Filename:  CONSTANTS.h
 *
 *    Description:  generated constants for the project
 *
 *        Version:  1.0
 *        Created:  22.02.2010 19:08:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  CONSTANTS_GEN_H_INC
#define  CONSTANTS_GEN_H_INC

#define sDDS_LOCAL_BYTEORDER_LITTLE_ENDIAN
//#define sDDS_LOCAL_BYTEORDER_BIG_ENDIAN
#define sDDS_NET_VERSION 0x01

#define sDDS_MAX_DW_APP 2

#define sDDS_MAX_DATA_READERS 7
#define sDDS_MAX_DATA_WRITERS 7

#define sDDS_NET_MAX_OUT_QUEUE 2
#define sDDS_NET_MAX_BUF_SIZE 30
#define sDDS_NET_MAX_LOCATOR_COUNT 10
#define sDDS_QOS_DW1_LATBUD 100
#define sDDS_QOS_DW2_LATBUD 500
#define sDDS_TOPIC_HAS_SUB
#define sDDS_TOPIC_HAS_PUB
#define sDDS_TOPIC_APP_MSG_COUNT 2
#define sDDS_TOPIC_MAX_COUNT 2
#define sDDS_MNG_WORKER_CYCLE_TIME 10000
#define sDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER 2

#endif   /* ----- #ifndef CONSTANTS_GEN_H_INC  ----- */
