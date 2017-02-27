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
 * @file      Geometry_PolyhedralSurface.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACE_H_

#include "Geometry.h"

bool_t
PolyhedralSurface_equals(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_disjoint(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_intersects(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_touches(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_crosses(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_within(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_contains(PolyhedralSurface_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurface_overlaps(PolyhedralSurface_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACE_H_ */
