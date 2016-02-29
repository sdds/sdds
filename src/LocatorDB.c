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

#include "sDDS.h"
#include "os-ssal/Mutex.h"
#include "Locator.h"
//#include "execinfo.h"

struct LocatorDB_t {

    // remember this sould be a pointer!
    Locator_t* pool[SDDS_NET_MAX_LOCATOR_COUNT];
    uint8_t freeLoc;
    Locator_t* toFind;
};

static struct LocatorDB_t db;
static Mutex_t* mutex;

/**
 * Initialises a Locator object
 *
 * @param _this Pointer to the object
 * @return is always SDDS_RT_OK
 */
static rc_t
s_init(Locator_t* _this);

#ifdef UTILS_DEBUG
static void
s_print() {
    printf("\n\nLOCATOR DB \n");
    for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
        char srcAddr[NI_MAXHOST];
        if (Locator_getAddress(db.pool[i], srcAddr) != SDDS_RT_OK) {
            srcAddr[0] = '0';
            srcAddr[1] = '\0';
        }
        char *type = "UNI";
        if (db.pool[i]->type == SDDS_LOCATOR_TYPE_MULTI) {
            type = "MUL";
        }

        printf("[(%p) %d,%d, %s, %s] ", db.pool[i], i, db.pool[i]->refCount, srcAddr, type);
    }
    printf("\nfreeLoc: %d\n\n", db.freeLoc);
}
#endif


rc_t
LocatorDB_init() {
    db.freeLoc = SDDS_NET_MAX_LOCATOR_COUNT;

    for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
        Network_createLocator(&(db.pool[i]));
        s_init(db.pool[i]);
    }

    Network_createLocator(&db.toFind);

    mutex = Mutex_create();
    if (mutex == NULL) {
        return SDDS_RT_FAIL;
    }

    if (Mutex_init(mutex) != SDDS_SSW_RT_OK) {
        return SDDS_RT_FAIL;
    }

    return SDDS_RT_OK;
}

// TODO this should be mutexed!
// or is only the network able to create new locators?
rc_t
LocatorDB_newLocator(Locator_t** loc) {
    assert(loc);
    Mutex_lock(mutex);
    if (db.freeLoc == 0) {
        Mutex_unlock(mutex);
        return SDDS_LOCATORDB_RT_NOFREELOCATORS;
    }
    // set is NULL so can use it to mark if we found one
    *loc = NULL;
    // search for locator witch is not referenced somewhere
    // could also be an empty one
    int i;
    for (i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
        // check if ref counter is zero
        if (db.pool[i]->refCount == 0) {
            *loc = db.pool[i];
            break;
        }
    }
    if (*loc == NULL) {
        db.freeLoc = 0;
        Mutex_unlock(mutex);
        Log_error("(%d) Cannot obtain free locator.\n", __LINE__);
        return SDDS_RT_FAIL;
    }
    (*loc)->type = SDDS_LOCATOR_TYPE_UNI;
    db.freeLoc--;
    (*loc)->refCount = 1;

    Mutex_unlock(mutex);

    return SDDS_RT_OK;
}

rc_t
LocatorDB_newMultiLocator(Locator_t** loc) {
    assert(loc);
    Mutex_lock(mutex);
    if (db.freeLoc == 0) {
        Mutex_unlock(mutex);
        return SDDS_LOCATORDB_RT_NOFREELOCATORS;
    }
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
    if (*loc == NULL) {
        db.freeLoc = 0;
        Mutex_unlock(mutex);
        Log_error("(%d) Cannot obtain free locator.\n", __LINE__);
        return SDDS_RT_FAIL;
    }

    (*loc)->type = SDDS_LOCATOR_TYPE_MULTI;
    db.freeLoc--;
    (*loc)->refCount = 1;

    Mutex_unlock(mutex);

    return SDDS_RT_OK;
}

rc_t
LocatorDB_newBroadLocator(Locator_t** loc) {
    assert(loc);
    Mutex_lock(mutex);
    if (db.freeLoc == 0) {
        return SDDS_LOCATORDB_RT_NOFREELOCATORS;
    }
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
    if (*loc == NULL) {
        db.freeLoc = 0;
        Mutex_unlock(mutex);
        Log_error("(%d) Cannot obtain free locator.\n", __LINE__);
        return SDDS_RT_FAIL;
    }

    (*loc)->type = SDDS_LOCATOR_TYPE_MULTI;
    db.freeLoc--;
    (*loc)->refCount = 1;

    Mutex_unlock(mutex);

    return SDDS_RT_OK;
}

static rc_t
s_freeLocator(Locator_t* loc) {
    // decrem refcounter if bigger than zero
    if (s_init(loc) == SDDS_RT_OK) {
        db.freeLoc++;
        return SDDS_RT_OK;
    }
    return SDDS_RT_FAIL;
}

rc_t
LocatorDB_isUsedLocator(Locator_t* loc) {
    // if ref counter > 0 return yes else no ;)
    Mutex_lock(mutex);
    if (loc->refCount > 0) {
        Mutex_unlock(mutex);
        return SDDS_LOCATORDB_RT_ISINUSE;
    }
    else{
        Mutex_unlock(mutex);
        return SDDS_RT_OK;
    }

}

rc_t
LocatorDB_findLocator(Locator_t* loc, Locator_t** result) {
    Mutex_lock(mutex);
    for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
        if ((db.pool[i]->refCount > 0) && (Locator_isEqual(loc, db.pool[i]) == true)) {
            *result = db.pool[i];
            Mutex_unlock(mutex);
            return SDDS_RT_OK;
        }
    }
    Mutex_unlock(mutex);
    return SDDS_RT_FAIL;
}

rc_t
LocatorDB_findLocatorByAddr(char *addr, Locator_t** result) {
    Mutex_lock(mutex);
    Network_setAddressToLocator(db.toFind, addr);
    for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
        if ((db.pool[i]->refCount > 0) && (Locator_isEqual(db.toFind, db.pool[i]) == true)) {
            *result = db.pool[i];
            Mutex_unlock(mutex);
            return SDDS_RT_OK;
        }
    }
    Mutex_unlock(mutex);
    return SDDS_RT_FAIL;
}

rc_t
LocatorDB_findLocatorByMcastAddr(char *addr, Locator_t** result) {
    Mutex_lock(mutex);
    Network_setMulticastAddressToLocator(db.toFind, addr);
    for (int i = 0; i < SDDS_NET_MAX_LOCATOR_COUNT; i++) {
        if ((db.pool[i]->refCount > 0) && (Locator_isEqual(db.toFind, db.pool[i]) == true)) {
            *result = db.pool[i];
            Mutex_unlock(mutex);
            return SDDS_RT_OK;
        }
    }
    Mutex_unlock(mutex);
    return SDDS_RT_FAIL;
}

#ifdef UTILS_TRACE
void
print_trace (int fun)
{
  void *array[10];
  size_t size;
  size_t i;

  size = backtrace (array, 10);

  switch (fun) {
  case 0:
      system("echo UP REF >> log.txt");
      break;
  case 1:
      system("echo DOWN REF >> log.txt");
      break;
  }

  for (i = 0; i < size; i++) {
      char command[150];
      sprintf(command, "addr2line -e linux_mcast1 %p >> log.txt", array[i]);
      system(command);
  }
}
#endif


void
Locator_upRef(Locator_t* _this) {

    if (_this == NULL) {
        return;
    }

    Mutex_lock(mutex);

#ifdef UTILS_DEBUG
    if (_this->refCount == 0) {
        Log_error("unauthorized upRef on %p:\n", _this);

        Locator_print(_this);
        s_print();
    }
#endif
#ifdef UTILS_TRACE
    print_trace(0);
#endif
    /*
     *	A locator with refCount 0 is free. New locators are obtained with refCount 1.
     *	upRef on a free Locator (refCount == 0) is illegal.
     *	If this happens with a non-free locator, probably you used the downRef operation wrong.
	 */
    if (_this->refCount == 0) {
    	Log_info("refCount is 0, this is a known bug. Please read the code commentary for detailed information.\n");
    }
    if (_this->refCount < 254) {
        _this->refCount++;
    }
    Mutex_unlock(mutex);
}
void
Locator_downRef(Locator_t* _this) {
    if (_this == NULL) {
        return;
    }

    Mutex_lock(mutex);
    print_trace(1);
    if (_this->refCount == 0) {
        Mutex_unlock(mutex);
        return;
    }
    if (_this->refCount > 0) {
        _this->refCount--;
    }
    if (_this->refCount == 0) {
        s_freeLocator(_this);
    }
    Mutex_unlock(mutex);
}

static rc_t
s_init(Locator_t* _this) {
    if (_this == NULL) {
        return SDDS_RT_BAD_PARAMETER;
    }

    _this->refCount = 0;
    _this->isEmpty = true;
    _this->isDest = false;
    _this->isSender = false;

    _this->type = SDDS_LOCATOR_TYPE_UNI;
    rc_t ret = Network_setPlatformAddressToLocator(_this);

    return ret;
}

uint8_t
LocatorDB_freeLocators() {
    return db.freeLoc;
}

#ifdef UTILS_DEBUG
void
LocatorDB_print() {
    Mutex_lock(mutex);
    s_print();
    Mutex_unlock(mutex);
}
#endif
