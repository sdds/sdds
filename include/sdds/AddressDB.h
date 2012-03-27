/*
 * =====================================================================================
 *
 *       Filename:  AddressDB.h
 *
 *    Description:  Address Database class. Holds the Locator obj in the system
 *
 *        Version:  1.0
 *        Created:  24.02.2010 12:56:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  ADDRESSDB_H_INC
#define  ADDRESSDB_H_INC

#include "Locator.h"

struct AddressDB_t{
    uint8_t count;
    Locator next = NULL;
};

typedef struct AddressDB_t* AddressDB;

AddressDB AddressDB_create();
rc_t AddressDB_add(AddressDB _this, Locator loc);
rc_t AddressDB_del(AddressDB _this, Locator loc);

#endif   /* ----- #ifndef ADDRESSDB_H_INC  ----- */

