/*
 * =====================================================================================
 *
 *       Filename:  SendState.h
 *
 *    Description:  header for the sending state maschine
 *
 *        Version:  1.0
 *        Created:  27.02.2010 14:38:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  SENDSTATE_INC
#define  SENDSTATE_INC
#include "sdds_types.h"

#define SDDS_SENDSTATE_FRESH	0
#define SDDS_SENDSTATE_ADDR	1
#define SDDS_SENDSTATE_DOMAIN	2
#define SDDS_SENDSTATE_TOPIC	3
#define SDDS_SENDSTATE_DATA	4


struct SendState_t{
    uint8_t state;
};


#endif   /* ----- #ifndef SENDSTATE_INC  ----- */
