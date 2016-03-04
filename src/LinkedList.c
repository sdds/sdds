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
LinkedList_first(List_t* list);
void*
LinkedList_next(List_t* list);
rc_t
LinkedList_add(List_t* list, void* data);
size_t
LinkedList_size(List_t* list);
rc_t
LinkedList_delete(List_t* list);
rc_t
LinkedList_delete_all(List_t* list);

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
    LinkedList_t* self = Memory_alloc(sizeof(LinkedList_t));
    assert(self);

    s_initNodePool();
    self->list.first_fn = LinkedList_first;
    self->list.next_fn = LinkedList_next;
    self->list.add_fn = LinkedList_add;
    self->list.size_fn = LinkedList_size;
    self->list.delete_fn = LinkedList_delete;
    self->list.delete_all_fn = LinkedList_delete_all;

    self->head = NULL;
    self->cursor = NULL;
    self->size = 0;

    return (List_t*) self;
}

// Provides the data of the first element, or NULL
void*
LinkedList_first(List_t* list) {
    assert(list);
    LinkedList_t* self = (LinkedList_t*) list;
    self->cursor = self->head;
    if(self->cursor) {
        return self->head->data;
    }
    else {
        return NULL;
    }
}

//  Provides the data of the next element, or NULL.
void*
LinkedList_next(List_t* list) {
    assert(list);
    LinkedList_t* self = (LinkedList_t*) list;

    if (self->cursor) {
        self->cursor = self->cursor->next;
    }
    else {
        self->cursor = self->head;
    }

    if (self->cursor) {
        return self->cursor->data;
    }
    else {
        return NULL;
    }
}

//  Adds data to the list, returns SDDS_RT_OK if successful otherwise SDDS_RT_FAIL.
rc_t
LinkedList_add(List_t* list, void* data) {
    assert(list);
    assert(data);
    LinkedList_t* self = (LinkedList_t*) list;

    Node_t* node = s_newNode();
    if(!node) {
        return SDDS_RT_FAIL;
    }
    node->data = data;
    node->isEmpty = false;
    node->next = NULL;

    if (self->head) {
        Node_t* tail = self->head;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = node;
    }
    else {
        self->head = node;
    }

    self->size++;
    self->cursor = NULL;
    return SDDS_RT_OK;
}

//  Delete the node currently pointed at by the cursor, returns SDDS_RT_OK if
//  successful otherwise SDDS_RT_FAIL. The cursor is invalidated by self
//  function.
rc_t
LinkedList_delete(List_t* list) {
    assert(list);
    LinkedList_t* self = (LinkedList_t*) list;

    Node_t *node, *prev = NULL;
    for(node = self->head; node != NULL; node = node->next) {
        if(node == self->cursor) {
            break;
        }
        prev = node;
    }
    if(node) {
        if (prev) {
            prev->next = node->next;
        }
        else {
            self->head = node->next;
        }
        node->data = NULL;
        node->next = NULL;
        node->isEmpty = true;

        self->cursor = prev;
        self->size--;
        return SDDS_RT_OK;
    }
    else {
        return SDDS_RT_FAIL;
    }
}

//  Return the size of the list.
size_t
LinkedList_size(List_t* list) {
    assert(list);
    LinkedList_t* self = (LinkedList_t*) list;
    return self->size;
}

rc_t
LinkedList_delete_all(List_t* list) {
    assert(list);
    LinkedList_t* self = (LinkedList_t*) list;

    Node_t* it = self->head;
    while (it != NULL) {
        Node_t* next = it->next;

        it->data = NULL;
        it->next = NULL;
        it->isEmpty = true;
        it = next;
    }
    self->head = NULL;
    self->cursor = NULL;
    self->size = 0;

    return SDDS_RT_OK;
}
