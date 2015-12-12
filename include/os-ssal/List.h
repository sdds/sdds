/*
 * List.h
 *
 *  Created on: Dec 11, 2015
 *      Author: olga
 */

#ifndef _LIST_H_
#define _LIST_H_

#include "sdds_types.h"
#include "SSW.h"

/**
 * List class
 */
struct List {
    void* (* List_first)(struct List *this); ///< Provides the data of the first element, or NULL.
    void* (* List_next)(struct List *this); ///< Provides the data of the next element, or NULL.
    ssw_rc_t (* List_add)(struct List *this, void* data); ///< Adds data to the list, return SDDS_SSW_RT_OK or SDDS_SSW_RT_FAIL.
    size_t (* List_size)(struct List* this);  ///< Return the size of the list.
    ssw_rc_t (* List_deleteAll)(struct List* this); ///< Deletes all elements from the list.
};
typedef struct List List_t;

/**
 *  ----------------------------------------------------------------------------
 * Initializes an array list.
 * @param[in]  The maximum size of the array list.
 * @return     Returns the pointer to the list on success, or NULL on failure.
 */
List_t*
List_initArrayList(size_t limit);

/**
 *  ----------------------------------------------------------------------------
 * Initializes a linked list.
 * @return     Returns the pointer to the list on success, or NULL on failure.
 */
List_t*
List_initLinkedList(void);

/**
 *  ----------------------------------------------------------------------------
 * Initializes a concurrent linked list.
 * @return     Returns the pointer to the list on success, or NULL on failure.
 */
List_t*
List_initConcurrentLinkedList(void);


#endif /* SDDS_INCLUDE_OS_SSAL_LIST_H_ */
