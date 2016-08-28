/*
 * DirectedEdgeList.c
 *
 *  Created on: Aug 28, 2016
 *      Author: o_dedi
 */

#include "DirectedEdgeList.h"

DirectedEdgeNode_t*
DirectedEdgeList_first(DirectedEdgeList_t* list) {
    assert(list);
    list->cursor = list->head;
    return list->cursor;
}


DirectedEdgeNode_t*
DirectedEdgeList_next(DirectedEdgeList_t* list) {
    assert(list);
    list->cursor = list->cursor->next;
    return list->cursor;
}

rc_t
DirectedEdgeList_add(DirectedEdgeList_t* list, DirectedEdge_t* data) {
    assert(list);
    assert(data);

    DirectedEdgeNode_t* node = DirectedEdgeList_first(list);
    DirectedEdgeNode_t* prev = NULL;
    while (node != NULL) {
        prev = node;
        node = DirectedEdgeList_next(list);
    }

    prev->next = malloc(sizeof(DirectedEdgeNode_t));
    prev->next->next = NULL;
    prev->next->data = data;

    list->size++;

    return SDDS_RT_OK;
}


size_t
DirectedEdgeList_size(DirectedEdgeList_t* list) {
    assert(list);
    return list->size;
}

rc_t
DirectedEdgeList_delete(DirectedEdgeList_t* list, DirectedEdgeNode_t* node) {
    assert(list);
    assert(node);

    DirectedEdgeNode_t* n = DirectedEdgeList_first(list);

    while (n != NULL) {
        if (n->next == node) {
            DirectedEdgeNode_t* del = n->next;
            n->next = del->next;
            free(del);
            list->size++;
            break;
        }
        n = DirectedEdgeList_next(list);
    }

    return SDDS_RT_OK;
}
