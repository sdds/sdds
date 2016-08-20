/*
 * GeometryStore.c
 *
 *  Created on: Aug 10, 2016
 *      Author: o_dedi
 */

#include "GeometryStore.h"
#include "sDDS.h"

#ifdef FEATURE_SDDS_GEOMETRY_ENABLED

Geometry_t*
GeometryStore_getGeometry(GeometryStore_t* self, GeometryId_t id) {
    for (uint16_t i = 0; i < self->size; i++) {
        if (self->entry[i].id == id) {
            return (Geometry_t*) &(self->entry[i].geometry);
        }
    }
    return NULL;
}

#endif
