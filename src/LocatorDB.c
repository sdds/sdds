/*
 * =====================================================================================
 *
 *       Filename:  LocatorDB.c
 *
 *    Description:  impl file for the abstract part of the AddressDB class
 *
 *        Version:  1.0
 *        Created:  03.03.2010 16:01:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include "LocatorDB.h"
#include "Locator.h"
#include "Network.h"

struct LocatorDB_t {

	// remember this sould be a pointer!
	Locator pool[SDDS_NET_MAX_LOCATOR_COUNT];
	uint8_t freeLoc;
};

static struct LocatorDB_t db;

rc_t LocatorDB_init() {
	db.freeLoc = SDDS_NET_MAX_LOCATOR_COUNT;

	for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
		Network_createLocator(&(db.pool[i]));
		Locator_init(db.pool[i]);
	}

	return SDDS_RT_OK;
}

// TODO this should be mutexed!
// or is only the network able to create new locators?
rc_t LocatorDB_newLocator(Locator* loc) {
    if (db.freeLoc == 0) {
		return SDDS_LOCATORDB_RT_NOFREELOCATORS;
	}
	db.freeLoc--;
	// set is NULL so can use it to mark if we found one
	*loc = NULL;
	// search for locator witch is not referenced somewhere
	// could also be an empty one
	for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
		// check if ref counter is zero
		if (db.pool[i]->refCount == 0) {
			*loc = db.pool[i];
			break;
		}
	}
    (*loc)->type = SDDS_LOCATOR_TYPE_UNI;
	(*loc)->next = NULL;

	return SDDS_RT_OK;
}

rc_t LocatorDB_newMultiLocator(Locator* loc) {
    if (db.freeLoc == 0) {
		return SDDS_LOCATORDB_RT_NOFREELOCATORS;
	}
	db.freeLoc--;
	// set is NULL so can use it to mark if we found one
	*loc = NULL;
	// search for locator witch is not referenced somewhere
	// could also be an empty one
	for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
		// check if ref counter is zero
		if (db.pool[i]->refCount == 0) {
			*loc = db.pool[i];
			break;
		}
	}
    (*loc)->type = SDDS_LOCATOR_TYPE_MULTI;
	(*loc)->next = NULL;

	return SDDS_RT_OK;
}

rc_t LocatorDB_freeLocator(Locator loc) {
	// decrem refcounter if bigger than zero
	if (loc->refCount > 0) {
		loc->refCount--;
	}
	if (loc->refCount == 0) {

		db.freeLoc++;
		Locator_init(loc);
	}

	return SDDS_RT_OK;
}

rc_t LocatorDB_isUsedLocator(Locator loc) {
	// if ref counter > 0 return yes else no ;)
	if (loc->refCount > 0)
		return SDDS_LOCATORDB_RT_ISINUSE;
	else
		return SDDS_RT_OK;

}

rc_t LocatorDB_findLocator(Locator toFind, Locator* result) {
	for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
		if (Locator_isEqual(toFind, db.pool[i]) == true) {
			*result = db.pool[i];
			return SDDS_RT_OK;
		}
	}
	return SDDS_RT_FAIL;
}
