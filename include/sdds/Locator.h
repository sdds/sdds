/*
 * =====================================================================================
 *
 *       Filename:  Locator.h
 *
 *    Description:  Locator interface for the abstractaion of the network
 *    address
 *
 *        Version:  1.0
 *        Created:  24.02.2010 12:54:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  LOCATOR_H_INC
#define  LOCATOR_H_INC
#include "sdds_types.h"
#include "List.h"

#define SDDS_LOCATOR_STATE_EMPTY        0x00
#define SDDS_LOCATOR_STATE_FILLED       0x01
#define SDDS_LOCATOR_STATE_REF          0x02

#define SDDS_LOCATOR_TYPE_UNI           0x00
#define SDDS_LOCATOR_TYPE_MULTI         0x01
#define SDDS_LOCATOR_TYPE_BROAD         0x02

typedef struct _Locator_t Locator_t;
struct _Locator_t {
    Locator_t* next;
    // TODO state reicht nicht ref counter?
    // wie geht das mit den bitmaps?
    // 4 bit refconter?
    unsigned int type   : 2;
    _Bool isEmpty               : 1;
    _Bool isDest                : 1;
    _Bool isSender              : 1;
    uint8_t refCount;
};


/**
 * Initialises a Locator object
 *
 * @param _this Pointer to the object
 * @return is always SDDS_RT_OK
 */
rc_t
Locator_init(Locator_t* _this);

/**
 * Checks if two Locator instances are equal or the same.
 * There is no distinction between these two cases.
 * The comparison depends on the specific implementation.
 * This method is abstract and have to be implemented in the
 * network specific class, like the "network module"
 *
 * @param l1 First Locator instance
 * @param l2 Second Locator instance
 * @return true or false, dependig if the two instances are equal or not
 */
bool_t
Locator_isEqual(Locator_t* l1, Locator_t* l2);

/**
 * Increments the reference counter of the locator instance.
 * @param _this Poniter to the Locator instance
 */
void
Locator_upRef(Locator_t* _this);


/**
 * decrements the reference counter of the locator instance.
 * If the counter becomes zero, the instance will be freed in the LocatorDB.
 *
 * @param _this Locator instance, which refcount shall be reduced
 */
void
Locator_downRef(Locator_t* _this);


/**
 * adds the given Locator instance to the end of the list.
 * If there is already an equal instance the refcounter of the instance in the
 *list
 * will be incremented and the given instance decremented.
 *
 * @param head Pointer to the head of the list
 * @param newL Pointer to the locator instance to insert
 * @return SDDS_RT_OK if insert was successful,
 * SDDS_RT_BAD_PARAMETER if one of the parameter is NULL
 */
rc_t
Locator_addToList(Locator_t* head, Locator_t* newL);

/**
 * Removes a Locator instance from the locator list.
 * If the locator is within the list, the reference counter
 * will be decremented and if the counter becomes zero the instance
 * will be freed.
 * If the given locator instance to remove is not the same as the
 * instance in the list, both instances reference counter will be
 * decremented
 *
 * @param head Pointer to the head of the list
 * @param newL Pointer to the locator instance to remove
 * @return SDDS_RT_OK if success,
 * SDDS_RT_FAIL if the given instance was not found in the list,
 * SDDS_RT_BAD_PARAMETER if one of the parameter is NULL
 */
rc_t
Locator_removeFromList(Locator_t* head, Locator_t* toDel);

rc_t
Locator_contains(List_t* list, Locator_t* l);

rc_t
Locator_getAddress(char* srcAddr);

rc_t
Locator_getAddressOfLocator(Locator_t* l, char* srcAddr);

#endif   /* ----- #ifndef LOCATOR_H_INC  ----- */
