/*
 * GeometryStore.c
 *
 *  Created on: Aug 10, 2016
 *      Author: o_dedi
 */

#include "GeometryStore.h"
#include "sDDS.h"

#ifdef FEATURE_SDDS_GEOMETRY_ENABLED

rc_t
GeometryStore_create(GeometryStore_t* self) {
    self->entry[0].id = GEO_WOHNZIMMER;
    self->entry[0].geometry = &s_geo_wohnzimmer;

    return SDDS_RT_OK;
}

Geometry_t*
GeometryStore_getGeometry(GeometryStore_t* self, GeometryId_t id) {
    for (uint16_t i = 0; i < self->size; i++) {
        if (self->entry[i].id == id) {
            return &(self->entry[i].geometry);
        }
    }
    return NULL;
}

#endif
