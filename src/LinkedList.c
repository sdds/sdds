/*
 * LinkedList.c
 *
 *  Created on: Dec 11, 2015
 *      Author: olga
 */

#include "List.h"
#include "os-ssal/Memory.h"
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

struct LinkedList {
    List_t list;
    Node_t* head;
    Node_t* cursor;
    size_t size;
};
typedef struct LinkedList LinkedList_t;

static Node_t nodePool[SDDS_MAX_LINKED_LIST_NODES];
static _Bool nodePoolInit = false;

//  Forward declaration of function pointers
void*
LinkedList_first(List_t* this);
void*
LinkedList_next(List_t* this);
rc_t
LinkedList_add(List_t* this, void* data);
size_t
LinkedList_size(List_t* this);
rc_t
LinkedList_delete_all(List_t* this);

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
List_initLinkedList(void) {
    LinkedList_t* this = Memory_alloc(sizeof(LinkedList_t));
    if (this == NULL) {
        return NULL;
    }

    s_initNodePool();
    this->list.first_fn = LinkedList_first;
    this->list.next_fn = LinkedList_next;
    this->list.add_fn = LinkedList_add;
    this->list.size_fn = LinkedList_size;
    this->list.delete_all_fn = LinkedList_delete_all;

    this->head = NULL;
    this->cursor = NULL;
    this->size = 0;

    return (List_t*) this;
}

// Provides the data of the first element, or NULL
void*
LinkedList_first(List_t* this) {
    if (this == NULL) {
        return NULL;
    }

    LinkedList_t* linkedList = (LinkedList_t*) this;
    if (linkedList->head == NULL) {
        return NULL;
    }
    else {
        linkedList->cursor = linkedList->head;
        void* data = linkedList->head->data;
        return data;
    }
}

//  Provides the data of the next element, or NUULL.
void*
LinkedList_next(List_t* this) {
    if (this == NULL) {
        return NULL;
    }

    LinkedList_t* linkedList = (LinkedList_t*) this;

    if (linkedList->cursor == NULL) {
        linkedList->cursor = linkedList->head;
    }

    if (linkedList->head == NULL) {
        return NULL;
    }
    else {
        linkedList->cursor = linkedList->cursor->next;
        if (linkedList->cursor == NULL) {
            return NULL;
        }
        void* data = linkedList->cursor->data;
        return data;
    }
}

//  Adds data to the list, return SDDS_RT_OK or SDDS_RT_FAIL.
rc_t
LinkedList_add(List_t* this, void* data) {
    if (this == NULL) {
        return SDDS_RT_FAIL;
    }

    LinkedList_t* linkedList = (LinkedList_t*) this;

    if (linkedList->head == NULL) {
        Node_t* n = s_newNode();
        if (n == NULL) {
            return SDDS_RT_FAIL;
        }

        n->isEmpty = false;
        n->data = data;
        n->next = NULL;
        linkedList->head = n;
        linkedList->size++;
        return SDDS_RT_OK;
    }
    else {
        Node_t* it = linkedList->head;
        while (it->next != NULL) {
            it = it->next;
        }

        Node_t* n = s_newNode();
        if (n == NULL) {
            return SDDS_RT_FAIL;
        }

        n->isEmpty = false;
        n->data = data;
        n->next = NULL;
        it->next = n;
        linkedList->size++;

        return SDDS_RT_OK;
    }
}

//  Return the size of the list.
size_t
LinkedList_size(List_t* this) {
    if (this == NULL) {
        return -1;
    }

    LinkedList_t* linkedList = (LinkedList_t*) this;
    size_t size = linkedList->size;
    return size;
}

rc_t
LinkedList_delete_all(List_t* this) {
    if (this == NULL) {
        return SDDS_RT_FAIL;
    }
    LinkedList_t* linkedList = (LinkedList_t*) this;

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

    return SDDS_RT_OK;
}
