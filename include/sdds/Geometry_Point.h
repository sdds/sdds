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
 * @file      Geometry_Point.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_POINT_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_POINT_H_

#include "Geometry.h"

bool_t
Point_equals(Point_t* self, Geometry_t* otherObject);

bool_t
Point_disjoint(Point_t* self, Geometry_t* otherObject);

bool_t
Point_intersects(Point_t* self, Geometry_t* otherObject);

bool_t
Point_touches(Point_t* self, Geometry_t* otherObject);

bool_t
Point_crosses(Point_t* self, Geometry_t* otherObject);

bool_t
Point_within(Point_t* self, Geometry_t* otherObject);

bool_t
Point_contains(Point_t* self, Geometry_t* otherObject);

bool_t
Point_overlaps(Point_t* self, Geometry_t* otherObject);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_POINT_H_ */
