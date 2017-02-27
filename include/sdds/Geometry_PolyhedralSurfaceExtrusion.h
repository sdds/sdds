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
 * @file      Geometry_PolyhedralSurfaceExtrusion.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACEEXTRUSION_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACEEXTRUSION_H_

#include "Geometry.h"

bool_t
PolyhedralSurfaceExtrusion_equals(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_disjoint(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_intersects(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_touches(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_crosses(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_within(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_contains(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);

bool_t
PolyhedralSurfaceExtrusion_overlaps(PolyhedralSurfaceExtrusion_t* self, Geometry_t* otherObject);



#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POLYHEDRALSURFACEEXTRUSION_H_ */
