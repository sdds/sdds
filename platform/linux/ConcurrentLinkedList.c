/*
 * LinkedList.c
 *
 *  Created on: Dec 11, 2015
 *      Author: olga
 */

#include "os-ssal/List.h"
#include "os-ssal/Memory.h"
#include "os-ssal/Mutex.h"
#include "DataSource.h"

#ifndef SDDS_MAX_LINKED_LIST_NODES
#define SDDS_MAX_LINKED_LIST_NODES 20
#endif

struct Node {
    struct Node* next;
    void* data;
    _Bool isEmpty : 1;
};
typedef struct Node Node_t;

struct ConcurrentLinkedList {
    List_t list;
    Node_t* head;
    Node_t* cursor;
    size_t size;
    Mutex_t* mutex;
};
typedef struct ConcurrentLinkedList ConcurrentLinkedList_t;

static Node_t nodePool[SDDS_MAX_LINKED_LIST_NODES];
static _Bool nodePoolInit = false;

//  Forward declaration of function pointers
void*
ConcurrentLinkedList_first(struct List *this);
void*
ConcurrentLinkedList_next(struct List *this);
ssw_rc_t
ConcurrentLinkedList_add(struct List *this, void* data);
size_t
ConcurrentLinkedList_size(struct List* this);
ssw_rc_t
ConcurrentLinkedList_deleteAll(struct List* this);

//  initialize node pool
static void
s_initNodePool() {
    if (!nodePoolInit) {
        for (int i = 0; i < SDDS_MAX_LINKED_LIST_NODES; i++) {
            nodePool[i].data = NULL;
            nodePool[i].next = NULL;
            nodePool[i].isEmpty = true;
        }
        nodePoolInit = true;
    }
}

//  get free node from node pool
static Node_t*
s_newNode() {
    for (int i = 0; i < SDDS_MAX_LINKED_LIST_NODES; i++) {
        if (nodePool[i].isEmpty) {
            return &nodePool[i];
        }
    }
    return NULL;
}

//  Initialize List
List_t*
List_initConcurrentLinkedList(void) {
    ConcurrentLinkedList_t* this = Memory_alloc(sizeof(ConcurrentLinkedList_t));
    if (this == NULL) {
        return NULL;
    }
    this->mutex = Mutex_create();
    if (this->mutex == NULL) {
        return NULL;
    }
    if (Mutex_init(this->mutex) != SDDS_SSW_RT_OK) {
        return NULL;
    }

    s_initNodePool();
    this->list.List_first = ConcurrentLinkedList_first;
    this->list.List_next = ConcurrentLinkedList_next;
    this->list.List_add = ConcurrentLinkedList_add;
    this->list.List_size = ConcurrentLinkedList_size;
    this->list.List_deleteAll = ConcurrentLinkedList_deleteAll;

    this->head = NULL;
    this->cursor = NULL;
    this->size = 0;

    return (List_t*) this;
}

// Provides the data of the first element, or NULL
void*
ConcurrentLinkedList_first(struct List *this) {
    if (this == NULL) {
        return NULL;
    }

    ConcurrentLinkedList_t* linkedList = (ConcurrentLinkedList_t*) this;
    Mutex_lock(linkedList->mutex);
    if (linkedList->head == NULL) {
        Mutex_unlock(linkedList->mutex);
        return NULL;
    }
    else {
        linkedList->cursor = linkedList->head;
        void* data = linkedList->head->data;
        Mutex_unlock(linkedList->mutex);
        return data;
    }
}

//  Provides the data of the next element, or NUULL.
void*
ConcurrentLinkedList_next(struct List *this) {
    if (this == NULL) {
        return NULL;
    }

    ConcurrentLinkedList_t* linkedList = (ConcurrentLinkedList_t*) this;

    Mutex_lock(linkedList->mutex);
    if (linkedList->cursor == NULL) {
        linkedList->cursor = linkedList->head;
    }

    if (linkedList->head == NULL) {
        Mutex_unlock(linkedList->mutex);
        return NULL;
    }
    else {
        linkedList->cursor = linkedList->cursor->next;
        if (linkedList->cursor == NULL) {
            Mutex_unlock(linkedList->mutex);
            return NULL;
        }
        void* data = linkedList->cursor->data;
        Mutex_unlock(linkedList->mutex);
        return data;
    }
}

//  Adds data to the list, return SDDS_SSW_RT_OK or SDDS_SSW_RT_FAIL.
ssw_rc_t
ConcurrentLinkedList_add(struct List *this, void* data) {
    if (this == NULL) {
        return SDDS_SSW_RT_FAIL;
    }

    ConcurrentLinkedList_t* linkedList = (ConcurrentLinkedList_t*) this;

    Mutex_lock(linkedList->mutex);
    if (linkedList->head == NULL) {
        Node_t* n = s_newNode();
        if (n == NULL) {
            Mutex_unlock(linkedList->mutex);
            return SDDS_SSW_RT_FAIL;
        }

        n->isEmpty = false;
        n->data = data;
        n->next = NULL;
        linkedList->head = n;
        linkedList->size++;
        Mutex_unlock(linkedList->mutex);
        return SDDS_SSW_RT_OK;
    }
    else {
        Node_t* it = linkedList->head;
        while (it->next != NULL) {
            it = it->next;
        }

        Node_t* n = s_newNode();
        if (n == NULL) {
            Mutex_unlock(linkedList->mutex);
            return SDDS_SSW_RT_FAIL;
        }

        n->isEmpty = false;
        n->data = data;
        n->next = NULL;
        it->next = n;
        linkedList->size++;
        Mutex_unlock(linkedList->mutex);

        return SDDS_SSW_RT_OK;
    }
}

//  Return the size of the list.
size_t
ConcurrentLinkedList_size(struct List* this) {
    if (this == NULL) {
        return SDDS_SSW_RT_FAIL;
    }

    ConcurrentLinkedList_t* linkedList = (ConcurrentLinkedList_t*) this;
    Mutex_lock(linkedList->mutex);
    size_t size = linkedList->size;
    Mutex_unlock(linkedList->mutex);
    return size;
}

ssw_rc_t
ConcurrentLinkedList_deleteAll(struct List* this) {
    if (this == NULL) {
        return SDDS_SSW_RT_FAIL;
    }
    ConcurrentLinkedList_t* linkedList = (ConcurrentLinkedList_t*) this;
    Mutex_lock(linkedList->mutex);

    Node_t* it = linkedList->head;
    while (it != NULL) {
        Node_t* next = it->next;

        it->data = NULL;
        it->next = NULL;
        it->isEmpty = true;
        it = next;
    }
    linkedList->head = NULL;
    linkedList->cursor = NULL;
    linkedList->size = 0;

    Mutex_unlock(linkedList->mutex);

    return SDDS_SSW_RT_OK;
}
