/*
 * ArrayList.c
 *
 *  Created on: Dec 11, 2015
 *      Author: olga
 */

#include "List.h"
#include "os-ssal/Memory.h"
#include "sdds_types.h"

struct ArrayList {
    List_t list;
    void** data;
    size_t cursor;
    size_t size;
    size_t limit;
};
typedef struct ArrayList ArrayList_t;

//  Forward declaration of function pointers
void*
ArrayList_first(List_t* this);
void*
ArrayList_next(List_t* this);
rc_t
ArrayList_add(List_t* this, void* data);
size_t
ArrayList_size(List_t* this);
rc_t
ArrayList_delete(List_t* this);
rc_t
ArrayList_delete_all(List_t* this);

//  Initialize List
List_t*
List_initArrayList(size_t limit) {
    if (limit == 0) {
        return NULL;
    }

    ArrayList_t* this = Memory_alloc(sizeof(ArrayList_t));
    if (this == NULL) {
        return NULL;
    }

    //  Allocate memory
    this->data = Memory_alloc(limit * sizeof(void*));
    if (this->data == NULL) {
        return NULL;
    }
    this->size = 0;
    this->limit = limit;
    this->cursor = 0;

    //  Set function pointer
    this->list.first_fn = ArrayList_first;
    this->list.next_fn = ArrayList_next;
    this->list.add_fn = ArrayList_add;
    this->list.size_fn = ArrayList_size;
    this->list.delete_fn = ArrayList_delete;
    this->list.delete_all_fn = ArrayList_delete_all;

    return (List_t*) this;
}

// Provides the data of the first element, or NULL
void*
ArrayList_first(List_t* this) {
    if (this == NULL) {
        return NULL;
    }

    ArrayList_t* arrayList = (ArrayList_t*) this;
    if (arrayList->size > 0) {
        arrayList->cursor = 0;
        return arrayList->data[0];
    }
    else {
        return NULL;
    }
}

//  Provides the data of the next element, or NUULL.
void*
ArrayList_next(List_t* this) {
    if (this == NULL) {
        return NULL;
    }

    ArrayList_t* arrayList = (ArrayList_t*) this;
    arrayList->cursor++;
    if (arrayList->cursor < arrayList->size) {
        return arrayList->data[arrayList->cursor];
    }
    else {
        return NULL;
    }
}

//  Adds data to the list, return SDDS_RT_OK or SDDS_RT_FAIL.
rc_t
ArrayList_add(List_t* this, void* data) {
    if (this == NULL) {
        return SDDS_RT_FAIL;
    }

    ArrayList_t* arrayList = (ArrayList_t*) this;
    if (arrayList->size >= arrayList->limit) {
        return SDDS_RT_FAIL;
    }

    arrayList->data[arrayList->size] = data;
    arrayList->size++;
    return SDDS_RT_OK;
}

//  Return the size of the list.
size_t
ArrayList_size(List_t* this) {
    if (this == NULL) {
        return -1;
    }

    ArrayList_t* arrayList = (ArrayList_t*) this;
    return arrayList->size;
}

//  Deletes the element at the cursor from the list.
rc_t
ArrayList_delete(List_t* this) {
    assert(this);
    return SDDS_RT_FAIL;
}

//  Deletes all elements from the list.
rc_t
ArrayList_delete_all(List_t* this) {
    if (this == NULL) {
        return SDDS_RT_FAIL;
    }
    ArrayList_t* arrayList = (ArrayList_t*) this;
    for (size_t i = 0; i < arrayList->size; i++) {
        arrayList->data[i] = NULL;
    }
    arrayList->size = 0;
    arrayList->cursor = 0;
    return SDDS_RT_OK;
}
