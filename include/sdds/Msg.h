/*
 * =====================================================================================
 *
 *       Filename:  Msg.h
 *
 *    Description:  Container class to hold the Data send or received in the dds 
 *    system
 *
 *        Version:  1.0
 *        Created:  02.03.2010 19:04:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  MSG_INC
#define  MSG_INC
#include "sdds_types.h"
#include "gen_constants.h"

#define SDDS_MSG_DIR_UNSPEZ	0
#define SDDS_MSG_DIR_INCOMMING	1
#define SDDS_MSG_DIR_OUTGOING	2

struct _Msg_t {
	//  Msg_t *next;D
	Data data;
	//    _Bool isEmpty	: 1;
	//    unsigned int dir	: 2;
	//    _Bool isRead	: 1;
	//    _Bool isLoaned	: 1;
	//    _Bool isNew		: 1;
};
typedef struct _Msg_t Msg_t;

struct MsgPool{
    Msg_t pool[SDDS_TOPIC_APP_MSG_COUNT];
    uint8_t start; // TODO generate datatype
    uint8_t count;   // TODO generate datatype
};


rc_t Msg_init(Msg_t *_this, Data dataBuffer);
rc_t Msg_getData(Msg_t *_this, Data* data);
// abstract -> impl in generated file
size_t Msg_getSize(Msg_t *_this);

#if defined(SDDS_TOPIC_HAS_PUB) || defined(FEATURE_SDDS_BUILTIN_TOPICS_ENABLED)
rc_t MsgPool_getFreeMsg(struct MsgPool* _this, Msg* msg);
rc_t MsgPool_getNextMsg(struct MsgPool* _this, Msg* msg);
rc_t MsgPool_getUnreadMsgCount(struct MsgPool* _this, uint8_t* count);
#endif

#endif   /* ----- #ifndef MSG_INC  ----- */
