/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      GeometryStore.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
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

rc_t
GeometryStore_create(GeometryStore_t* self);

Geometry_t*
GeometryStore_getGeometry(GeometryStore_t* self, GeometryId_t id);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRYSTORE_H_ */
