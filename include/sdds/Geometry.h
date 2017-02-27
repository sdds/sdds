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
 * @file      Geometry.h
 * @author    Olga Dedi
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * TODO
 */



#ifndef SDDS_INCLUDE_SDDS_GEOMETRY_H_
#define SDDS_INCLUDE_SDDS_GEOMETRY_H_

#include "sdds_types.h"

typedef uint8_t GeometryType_t;
typedef uint16_t GeoMeasurement_t;

/**
 * Geometry Types Bitmask
 */
#define GEO_TYPE_POINT                          (1 << 0)
#define GEO_TYPE_CURVE                          (1 << 1)
#define GEO_TYPE_LINESTRING                     GEO_TYPE_CURVE | (1 << 2)
#define GEO_TYPE_LINE                           GEO_TYPE_CURVE | (1 << 3)
#define GEO_TYPE_LINERING                       GEO_TYPE_LINESTRING | (1 << 3)
#define GEO_TYPE_SURFACE                        (1 << 2)
#define GEO_TYPE_BASICSHAPE                     GEO_TYPE_SURFACE | (1 << 3)
#define GEO_TYPE_ELLIPSE                        (uint8_t)(GEO_TYPE_BASICSHAPE | (1 << 4))
#define GEO_TYPE_SQUARE                         (uint8_t)(GEO_TYPE_BASICSHAPE | (1 << 5))
#define GEO_TYPE_POLYGON                        GEO_TYPE_SURFACE | (1 << 4)
#define GEO_TYPE_POLYHEDRALSURFACE              GEO_TYPE_SURFACE | (1 << 5)
#define GEO_TYPE_GEOMETRYCOLLECTION             (1 << 6)
#define GEO_TYPE_EXTRUSION                      (1 << 7)
#define GEO_TYPE_ELLIPSE_EXTRUSION              GEO_TYPE_ELLIPSE | GEO_TYPE_EXTRUSION
#define GEO_TYPE_SQUARE_EXTRUSION               GEO_TYPE_SQUARE | GEO_TYPE_EXTRUSION
#define GEO_TYPE_POLYGON_EXTRUSION              GEO_TYPE_POLYGON | GEO_TYPE_EXTRUSION
#define GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION    GEO_TYPE_POLYHEDRALSURFACE | GEO_TYPE_EXTRUSION


#define GEO_MM_ACCURACY     0
#define GEO_CM_ACCURACY     1
#define GEO_DM_ACCURACY     2
#define GEO_1M_ACCURACY     3
#define GEO_10M_ACCURACY    4
#define GEO_100M_ACCURACY   5
#define GEO_KM_ACCURACY     6

#ifndef GEO_MEASUEMENT_ACCURACY
#define GEO_MEASUEMENT_ACCURACY GEO_CM_ACCURACY
#endif

struct Geometry {
    GeometryType_t type;
};
typedef struct Geometry Geometry_t;

struct Point {
    Geometry_t geometry;
    GeoMeasurement_t x;
    GeoMeasurement_t y;
    GeoMeasurement_t z;
};
typedef struct Point Point_t;

struct LineString {
    Geometry_t geometry;
    Point_t* vertices;
    uint16_t vertexCount;
};
typedef struct LineString LineString_t;

struct Line {
    LineString_t lineString;
};
typedef struct Line Line_t;

struct LineRing {
    LineString_t lineString;
};
typedef struct LineRing LineRing_t;

struct Polygon {
    Geometry_t geometry;
    LineRing_t* elements;
    uint16_t elementCount;
};
typedef struct Polygon Polygon_t;

struct BasicShape {
    Geometry_t geometry;
    Point_t vertex;
    GeoMeasurement_t width;
    GeoMeasurement_t length;
};
typedef struct BasicShape BasicShape_t;

struct Ellipse {
    BasicShape_t basicShape;
};
typedef struct Ellipse Ellipse_t;

struct Square {
    BasicShape_t basicShape;
};
typedef struct Square Square_t;

struct PolyhedralSurface {
    Geometry_t geometry;
    Polygon_t* patches;
    uint16_t patchCount;
};
typedef struct PolyhedralSurface PolyhedralSurface_t;

struct EllipseExtrusion {
    Ellipse_t ellipse;
    GeoMeasurement_t height;
};
typedef struct EllipseExtrusion EllipseExtrusion_t;

struct SquareExtrusion {
    Square_t square;
    GeoMeasurement_t height;
};
typedef struct SquareExtrusion SquareExtrusion_t;

struct PolygonExtrusion {
    Polygon_t polygon;
    GeoMeasurement_t height;
};
typedef struct PolygonExtrusion PolygonExtrusion_t;

struct PolyhedralSurfaceExtrusion {
    PolyhedralSurface_t polyhedralSurface;
    GeoMeasurement_t height;
};
typedef struct PolyhedralSurfaceExtrusion PolyhedralSurfaceExtrusion_t;

bool_t
Geometry_eaquals(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_disjoint(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_intersects(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_touches(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_crosses(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_within(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_contains(Geometry_t* self, Geometry_t* otherObject);

bool_t
Geometry_overlaps(Geometry_t* self, Geometry_t* otherObject);

Polygon_t*
BasicShape_asPolygon(BasicShape_t* self);

void
Geometry_initSquare(Square_t* self);

void
Geometry_initEllipse(Ellipse_t* self);

void
Geometry_initPoint(Point_t* self);

#endif /* SDDS_INCLUDE_SDDS_GEOMETRY_H_ */
