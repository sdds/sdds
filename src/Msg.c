/*
 * =====================================================================================
 *
 *       Filename:  Msg.c
 *
 *    Description:  impl file for the abstract Msg class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 09:11:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "Msg.h"
#include "sdds_types.h"

rc_t Msg_init(Msg _this, Data dataBuffer)
{

    _this->next = NULL;
    if (dataBuffer != NULL) {
    	_this->data = dataBuffer;
    }
    _this->isEmpty = true;
    _this->dir = SDDS_MSG_DIR_UNSPEZ;
    _this->isRead = false;
    _this->isLoaned = false;
    _this->isNew = false;

    return SDDS_RT_OK;
} 

