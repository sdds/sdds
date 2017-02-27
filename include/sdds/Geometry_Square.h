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
 * @file      Geometry_Square.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_SQUARE_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_SQUARE_H_

#include "Geometry.h"

bool_t
Square_equals(Square_t* self, Geometry_t* otherObject);

bool_t
Square_disjoint(Square_t* self, Geometry_t* otherObject);

bool_t
Square_intersects(Square_t* self, Geometry_t* otherObject);

bool_t
Square_touches(Square_t* self, Geometry_t* otherObject);

bool_t
Square_crosses(Square_t* self, Geometry_t* otherObject);

bool_t
Square_within(Square_t* self, Geometry_t* otherObject);

bool_t
Square_contains(Square_t* self, Geometry_t* otherObject);

bool_t
Square_overlaps(Square_t* self, Geometry_t* otherObject);

Polygon_t*
Square_asPolygon(Square_t* self);


#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_SQUARE_H_ */
