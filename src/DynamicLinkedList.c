/*
 * DynamicLinkedList.c
 *
 *  Created on: Aug 28, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "List.h"
#include "os-ssal/Memory.h"

struct Node {
    struct Node* next;
    void* data;
};
typedef struct Node Node_t;

struct DynamicLinkedList {
    List_t list;
    Node_t* head;
    Node_t* cursor;
    size_t size;
};
typedef struct DynamicLinkedList DynamicLinkedList_t;

void*
DynamicLinkedList_first(List_t* self);
void*
DynamicLinkedList_next(List_t* self);
rc_t
DynamicLinkedList_add(List_t* self, void* data);
size_t
DynamicLinkedList_size(List_t* self);
rc_t
DynamicLinkedList_delete(List_t* self);
rc_t
DynamicLinkedList_delete_all(List_t* self);


//  Initialize List
List_t*
List_initDynamicLinkedList(void) {
    DynamicLinkedList_t* self = Memory_alloc(sizeof(DynamicLinkedList_t));
    assert(self);

    self->list.first_fn = DynamicLinkedList_first;
    self->list.next_fn = DynamicLinkedList_next;
    self->list.add_fn = DynamicLinkedList_add;
    self->list.size_fn = DynamicLinkedList_size;
    self->list.delete_fn = DynamicLinkedList_delete;
    self->list.delete_all_fn = DynamicLinkedList_delete_all;

    self->head = NULL;
    self->cursor = NULL;
    self->size = 0;

    return (List_t*) self;
}

void*
DynamicLinkedList_first(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) self;
    list->cursor = list->head;
    if(list->cursor) {
        return list->head->data;
    }
    else {
        return NULL;
    }
}

void*
DynamicLinkedList_next(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) self;
    if (list->cursor) {
        list->cursor = list->cursor->next;
    }
    else {
        list->cursor = list->head;
    }

    if (list->cursor) {
        return list->cursor->data;
    }
    else {
        return NULL;
    }
}

rc_t
DynamicLinkedList_add(List_t* self, void* data) {
    assert(self);
    assert(data);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) self;

    Node_t* node = malloc(sizeof(Node_t));
    if(!node) {
        return SDDS_RT_FAIL;
    }
    node->data = data;
    node->next = NULL;

    if (list->head) {
        Node_t* tail = list->head;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = node;
    }
    else {
        list->head = node;
    }

    list->size++;
    list->cursor = NULL;

    return SDDS_RT_OK;
}


size_t
DynamicLinkedList_size(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) self;
    return list->size;
}

rc_t
DynamicLinkedList_delete(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) self;
    Node_t* node = list->head;

    while (node != NULL) {
        if (node->next == list->cursor) {
            node->next = list->cursor->next;
            free(list->cursor);
            list->cursor = node;
            list->size++;
            break;
        }
        node = node->next;
    }

    return SDDS_RT_OK;
}

rc_t
DynamicLinkedList_delete_all(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) self;

    Node_t* node = list->head;
    while (node != NULL) {
        Node_t* del = node;
        node = node->next;
        free(del);
    }
    list->head = NULL;
    list->cursor = NULL;
    list->size = 0;

    return SDDS_RT_OK;
}
