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
 * @file      Geometry_Ellipse.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSE_H_

#include "Geometry.h"

bool_t
Ellipse_equals(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_disjoint(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_intersects(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_touches(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_crosses(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_within(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_contains(Ellipse_t* self, Geometry_t* otherObject);

bool_t
Ellipse_overlaps(Ellipse_t* self, Geometry_t* otherObject);

Polygon_t*
Ellipse_asPolygon(Ellipse_t* self);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_ELLIPSE_H_ */
