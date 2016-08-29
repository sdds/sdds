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
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) list;
    list->cursor = list->head;
    return list->cursor->data;
}

void*
DynamicLinkedList_next(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) list;
    list->cursor = list->cursor->next;
    return list->cursor->data;
}

rc_t
DynamicLinkedList_add(List_t* self, void* data) {
    assert(self);
    assert(data);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) list;


    if (list->head == NULL) {
        list->head = malloc(sizeof(Node_t));
        list->head->next = NULL;
        list->head->data = data;
        list->size = 1;

        return SDDS_RT_OK;
    }

    Node_t* node = list->head;
    Node_t* prev = NULL;
    while (node != NULL) {
        prev = list->cursor;
        node = list->cursor->next;
    }

    prev->next = malloc(sizeof(Node_t));
    prev->next->next = NULL;
    prev->next->data = data;

    list->size++;

    return SDDS_RT_OK;
}


size_t
DynamicLinkedList_size(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) list;
    return list->size;
}

rc_t
DynamicLinkedList_delete(List_t* self) {
    assert(self);
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) list;
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
    DynamicLinkedList_t* list = (DynamicLinkedList_t*) list;

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
