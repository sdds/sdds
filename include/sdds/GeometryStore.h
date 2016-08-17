/*
 * GeometryStore.h
 *
 *  Created on: Aug 10, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_GEOMETRYSTORE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRYSTORE_H_

#include "Geometry.h"

typedef uint16_t GeometryId_t;

struct GeometryStoreEntry {
    GeometryId_t id;
    Geometry_t* geometry;
};
typedef struct GeometryStoreEntry GeometryStoreEntry_t;

struct GeometryStore {
    GeometryStoreEntry_t* entry;
    uint16_t size;
};
typedef struct GeometryStore GeometryStore_t;

GeometryStore_t*
GeometryStore_create();

Geometry_t*
GeometryStore_getGeometry(GeometryStore_t* self, GeometryId_t id);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRYSTORE_H_ */
