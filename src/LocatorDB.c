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
	Locator_t* pool[SDDS_NET_MAX_LOCATOR_COUNT];
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
rc_t LocatorDB_newLocator(Locator_t** loc) {
//	printf(" !!!!! NEW LOC db.freeLoc = %d\n", db.freeLoc);
//	for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
//		// check if ref counter is zero
//		printf("------------------------> db.pool[%i]->refCount = %d\n", i, db.pool[i]->refCount);
//	}

    if (db.freeLoc == 0) {
		return SDDS_LOCATORDB_RT_NOFREELOCATORS;
	}
	db.freeLoc--;
	// set is NULL so can use it to mark if we found one
	*loc = NULL;
	// search for locator witch is not referenced somewhere
	// could also be an empty one
	int i;
	for (i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
		// check if ref counter is zero
		if (db.pool[i]->refCount == 0) {
			*loc = db.pool[i];
//			printf(" FOUND FREE LOC\n");
			break;
		}
	}
	if (i == SDDS_NET_MAX_LOCATOR_COUNT) {
		printf(" NOT FOUND FREE LOC\n");
	}
    (*loc)->type = SDDS_LOCATOR_TYPE_UNI;
	(*loc)->next = NULL;

	return SDDS_RT_OK;
}

rc_t LocatorDB_newMultiLocator(Locator_t** loc) {
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

rc_t LocatorDB_newBroadLocator(Locator_t** loc) {
    if (db.freeLoc == 0) {
		return SDDS_LOCATORDB_RT_NOFREELOCATORS;
	}
	db.freeLoc--;
	// set is NULL so can use it to mark if we found one
	//*loc = NULL;
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

rc_t LocatorDB_freeLocator(Locator_t* loc) {
	// decrem refcounter if bigger than zero
	if (loc->refCount > 0) {
		loc->refCount--;
	}
	if (loc->refCount == 0) {
		Locator_init(loc);
		db.freeLoc++;
//		printf(" !!!!!! LOC FREED db.freeLoc = %d\n", db.freeLoc);
//		for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
//			// check if ref counter is zero
//			printf("------------------------> db.pool[%i]->refCount = %d\n", i, db.pool[i]->refCount);
//		}
	}

	return SDDS_RT_OK;
}

rc_t LocatorDB_isUsedLocator(Locator_t* loc) {
	// if ref counter > 0 return yes else no ;)
	if (loc->refCount > 0)
		return SDDS_LOCATORDB_RT_ISINUSE;
	else
		return SDDS_RT_OK;

}

rc_t LocatorDB_findLocator(Locator_t* toFind, Locator_t** result) {
	for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
		if (Locator_isEqual(toFind, db.pool[i]) == true) {
			*result = db.pool[i];
			return SDDS_RT_OK;
		}
	}
	return SDDS_RT_FAIL;
}
