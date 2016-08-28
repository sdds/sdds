/*
 * DirectedEdgeList.h
 *
 *  Created on: Aug 28, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_DIRECTEDEDGELIST_H_
#define SDDS_INCLUDE_SDDS_DIRECTEDEDGELIST_H_

#include "sDDS.h"

struct DirectedEdgeNode {
    struct DirectedEdgeNode* next;
    DirectedEdge_t* data;
};
typedef struct DirectedEdgeNode DirectedEdgeNode_t;

struct DirectedEdgeList {
    DirectedEdgeNode_t head;
    DirectedEdgeNode_t* cursor;
    size_t size;
};
typedef struct DirectedEdgeList DirectedEdgeList_t;

DirectedEdgeNode_t*
DirectedEdgeList_first(DirectedEdgeList_t* list);
DirectedEdgeNode_t*
DirectedEdgeList_next(DirectedEdgeList_t* list);
rc_t
DirectedEdgeList_add(DirectedEdgeList_t* list, DirectedEdge_t* data);
size_t
DirectedEdgeList_size(DirectedEdgeList_t* list);
rc_t
DirectedEdgeList_delete(DirectedEdgeList_t* list, DirectedEdgeNode_t* node);

#endif /* SDDS_INCLUDE_SDDS_DIRECTEDEDGELIST_H_ */
