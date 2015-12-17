/*
 * List.h
 *
 *  Created on: Dec 11, 2015
 *      Author: olga
 */

#ifndef _LIST_H_
#define _LIST_H_

#include "sdds_types.h"

typedef struct List List_t;
typedef void* (List_first_fn)(List_t* self);
typedef void* (List_next_fn)(List_t* self);
typedef rc_t (List_add_fn)(List_t* self, void* data);
typedef size_t (List_size_fn)(List_t* self);
typedef rc_t (List_delete_all_fn)(List_t* self);

/**
 * List class
 */
struct List {
    List_first_fn *first_fn; ///< Provides the data of the first element, or NULL.
    List_next_fn *next_fn; ///< Provides the data of the next element, or NULL.
    List_add_fn *add_fn; ///< Adds data to the list, return SDDS_SSW_RT_OK or SDDS_SSW_RT_FAIL.
    List_size_fn *size_fn;  ///< Return the size of the list.
    List_delete_all_fn *delete_all_fn; ///< Deletes all elements from the list.
};

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
