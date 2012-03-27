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

#define SDDS_MSG_DIR_UNSPEZ	0
#define SDDS_MSG_DIR_INCOMMING	1
#define SDDS_MSG_DIR_OUTGOING	2

typedef struct Msg_t* Msg;
struct Msg_t {
    Msg next;
    Data data;
    _Bool isEmpty	: 1;
    unsigned int dir	: 2;
    _Bool isRead	: 1;
    _Bool isLoaned	: 1;
    _Bool isNew		: 1;
};

rc_t Msg_init(Msg _this, Data dataBuffer);
rc_t Msg_getData(Msg _this, Data* data);
// abstract -> impl in generated file
size_t Msg_getSize(Msg _this);

#endif   /* ----- #ifndef MSG_INC  ----- */
