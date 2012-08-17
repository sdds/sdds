/*
 * Locator.c
 *
 *  Created on: 10 Aug 2012
 *      Author: k_beckma
 */

#include "Locator.h"
#include "LocatorDB.h"

void Locator_upRef(Locator _this) {

	if (_this == NULL) return;

	if (_this->refCount < 254) {
		_this->refCount++;
	}
}
void Locator_downRef(Locator _this) {
	if (_this == NULL) return;

	if (_this->refCount > 0) {
		_this->refCount--;
	}
	if (_this->refCount == 0) {
		LocatorDB_freeLocator(_this);
	}
}

rc_t Locator_init(Locator _this) {

	if (_this == NULL)
		return SDDS_RT_BAD_PARAMETER;

	_this->next = NULL;
	_this->refCount = 0;
	_this->isEmpty = true;
	_this->isDest = false;
	_this->isSender = false;

	return SDDS_RT_OK;
}


rc_t Locator_addToList(Locator head, Locator newL) {

	if (head == NULL || newL == NULL) {
		return SDDS_RT_BAD_PARAMETER;
	}

	Locator tmp = head;

	while (head != NULL) {
		// locator is allready in the list
		if (Locator_isEqual(head, newL) == true) {
			// up ref of loc in list
			Locator_upRef(head);
			// down ref of locator to insert
			Locator_downRef(newL);
			return SDDS_RT_OK;
		}
		tmp = head;
		head = head->next;
	}
	// add to the end
	tmp->next = newL;
	// take care its the end
	newL->next = NULL;

	Locator_upRef(newL);

	return SDDS_RT_OK;
}

rc_t Locator_removeFromList(Locator head, Locator toDel) {

	if (head == NULL || toDel == NULL)
		return SDDS_RT_BAD_PARAMETER;

	Locator tmp = NULL;
	Locator prev = head;

	while (head != NULL) {
		// found it
		if (Locator_isEqual(head, toDel) == true) {
			tmp = head;
			break;
		}
		prev = head;
		head = head->next;
	}

	// nothing found
	if (tmp == NULL) {
		return SDDS_RT_FAIL;
	}

	// chain out
	prev->next = tmp->next;

	tmp->next = NULL;

	// check if they are the same or just equal
	// if just equal we downref both of them
	if (toDel != tmp) {
		Locator_downRef(toDel);
		toDel->next = NULL;
	}
	Locator_downRef(tmp);

	return SDDS_RT_OK;
}
