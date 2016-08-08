/*
 * Geometry_Ellipse.c
 *
 *  Created on: Aug 2, 2016
 *      Author: o_dedi
 */

#include "Geometry_Ellipse.h"
#include <stdlib.h>
#include <math.h>

static bool_t
s_Point_equals(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_disjoint(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_intersects(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_touches(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_crosses(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_within(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_contains(Ellipse_t* self, Point_t* otherObject);
static bool_t
s_Point_overlaps(Ellipse_t* self, Point_t* otherObject);

static bool_t
s_LineString_equals(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_disjoint(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_intersects(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_touches(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_crosses(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_within(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_contains(Ellipse_t* self, LineString_t* otherObject);
static bool_t
s_LineString_overlaps(Ellipse_t* self, LineString_t* otherObject);

static bool_t
s_Line_equals(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_disjoint(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_intersects(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_touches(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_crosses(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_within(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_contains(Ellipse_t* self, Line_t* otherObject);
static bool_t
s_Line_overlaps(Ellipse_t* self, Line_t* otherObject);

static bool_t
s_LineRing_equals(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_disjoint(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_intersects(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_touches(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_crosses(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_within(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_contains(Ellipse_t* self, LineRing_t* otherObject);
static bool_t
s_LineRing_overlaps(Ellipse_t* self, LineRing_t* otherObject);

static bool_t
s_Polygon_equals(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_disjoint(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_intersects(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_touches(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_crosses(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_within(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_contains(Ellipse_t* self, Polygon_t* otherObject);
static bool_t
s_Polygon_overlaps(Ellipse_t* self, Polygon_t* otherObject);

static bool_t
s_Ellipse_equals(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_disjoint(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_intersects(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_touches(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_crosses(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_within(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_contains(Ellipse_t* self, Ellipse_t* otherObject);
static bool_t
s_Ellipse_overlaps(Ellipse_t* self, Ellipse_t* otherObject);

static bool_t
s_Square_equals(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_disjoint(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_intersects(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_touches(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_crosses(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_within(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_contains(Ellipse_t* self, Square_t* otherObject);
static bool_t
s_Square_overlaps(Ellipse_t* self, Square_t* otherObject);

static bool_t
s_PolyhedralSurface_equals(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_disjoint(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_intersects(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_touches(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_crosses(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_within(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_contains(Ellipse_t* self, PolyhedralSurface_t* otherObject);
static bool_t
s_PolyhedralSurface_overlaps(Ellipse_t* self, PolyhedralSurface_t* otherObject);

static bool_t
s_EllipseExtrusion_equals(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_disjoint(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_intersects(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_touches(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_crosses(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_within(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_contains(Ellipse_t* self, EllipseExtrusion_t* otherObject);
static bool_t
s_EllipseExtrusion_overlaps(Ellipse_t* self, EllipseExtrusion_t* otherObject);

static bool_t
s_SquareExtrusion_equals(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_disjoint(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_intersects(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_touches(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_crosses(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_within(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_contains(Ellipse_t* self, SquareExtrusion_t* otherObject);
static bool_t
s_SquareExtrusion_overlaps(Ellipse_t* self, SquareExtrusion_t* otherObject);

static bool_t
s_PolygonExtrusion_equals(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_disjoint(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_intersects(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_touches(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_crosses(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_within(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_contains(Ellipse_t* self, PolygonExtrusion_t* otherObject);
static bool_t
s_PolygonExtrusion_overlaps(Ellipse_t* self, PolygonExtrusion_t* otherObject);

static bool_t
s_PolyhedralSurfaceExtrusion_equals(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_disjoint(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_intersects(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_touches(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_crosses(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_within(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_contains(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);
static bool_t
s_PolyhedralSurfaceExtrusion_overlaps(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject);

/******************************************************
 *                  Implementation
 ******************************************************/

bool_t
Ellipse_equals(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_equals(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_equals(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_equals(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_equals(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_equals(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_equals(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_equals(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_equals(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_equals(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_equals(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_equals(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_equals(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_disjoint(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_disjoint(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_disjoint(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_disjoint(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_disjoint(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_disjoint(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_disjoint(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_disjoint(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_disjoint(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_disjoint(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_disjoint(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_disjoint(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_disjoint(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_intersects(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_intersects(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_intersects(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_intersects(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_intersects(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_intersects(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_intersects(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_intersects(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_intersects(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_intersects(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_intersects(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_intersects(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_intersects(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_touches(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_touches(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_touches(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_touches(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_touches(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_touches(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_touches(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_touches(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_touches(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_touches(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_touches(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_touches(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_touches(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_crosses(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_crosses(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_crosses(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_crosses(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_crosses(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_crosses(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_crosses(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_crosses(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_crosses(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_crosses(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_crosses(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_crosses(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_crosses(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_within(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_within(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_within(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_within(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_within(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_within(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
    {
        bool_t ret = s_Square_within(self, (Square_t*)otherObject);
        if (ret) {
            printf("Ellipse([%u, %u, %u], %u, %u) WHITHIN Square([%u, %u, %u], %u, %u): TRUE\n",
                    self->basicShape.vertex.x,
                    self->basicShape.vertex.y,
                    self->basicShape.vertex.z,
                    self->basicShape.width,
                    self->basicShape.length,
                    ((Square_t*)otherObject)->basicShape.vertex.x,
                    ((Square_t*)otherObject)->basicShape.vertex.y,
                    ((Square_t*)otherObject)->basicShape.vertex.z,
                    ((Square_t*)otherObject)->basicShape.width,
                    ((Square_t*)otherObject)->basicShape.length);
        } else {
            printf("Ellipse([%u, %u, %u], %u, %u) WHITHIN Square([%u, %u, %u], %u, %u): FALSE\n",
                    self->basicShape.vertex.x,
                    self->basicShape.vertex.y,
                    self->basicShape.vertex.z,
                    self->basicShape.width,
                    self->basicShape.length,
                    ((Square_t*)otherObject)->basicShape.vertex.x,
                    ((Square_t*)otherObject)->basicShape.vertex.y,
                    ((Square_t*)otherObject)->basicShape.vertex.z,
                    ((Square_t*)otherObject)->basicShape.width,
                    ((Square_t*)otherObject)->basicShape.length);
        }
        return ret;
    }
    case GEO_TYPE_POLYGON:
        return s_Polygon_within(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_within(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_within(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_within(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_within(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_within(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_contains(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_contains(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_contains(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_contains(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_contains(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_contains(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
        return s_Square_contains(self, (Square_t*)otherObject);

    case GEO_TYPE_POLYGON:
        return s_Polygon_contains(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_contains(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_contains(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_contains(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_contains(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_contains(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

bool_t
Ellipse_overlaps(Ellipse_t* self, Geometry_t* otherObject) {
    switch (otherObject->type) {
    case GEO_TYPE_POINT:
        return s_Point_overlaps(self, (Point_t*)otherObject);

    case GEO_TYPE_LINESTRING:
        return s_LineString_overlaps(self, (LineString_t*)otherObject);

    case GEO_TYPE_LINE:
        return s_Line_overlaps(self, (Line_t*)otherObject);

    case GEO_TYPE_LINERING:
        return s_LineRing_overlaps(self, (LineRing_t*)otherObject);

    case GEO_TYPE_ELLIPSE:
        return s_Ellipse_overlaps(self, (Ellipse_t*)otherObject);

    case GEO_TYPE_SQUARE:
    {
        bool_t ret = s_Square_overlaps(self, (Square_t*)otherObject) && !s_Square_within(self, (Square_t*)otherObject);
        if (ret) {
            printf("Ellipse([%u, %u, %u], %u, %u) OVERLAPS Square([%u, %u, %u], %u, %u): TRUE\n",
                    self->basicShape.vertex.x,
                    self->basicShape.vertex.y,
                    self->basicShape.vertex.z,
                    self->basicShape.width,
                    self->basicShape.length,
                    ((Square_t*)otherObject)->basicShape.vertex.x,
                    ((Square_t*)otherObject)->basicShape.vertex.y,
                    ((Square_t*)otherObject)->basicShape.vertex.z,
                    ((Square_t*)otherObject)->basicShape.width,
                    ((Square_t*)otherObject)->basicShape.length);
        } else {
            printf("Ellipse([%u, %u, %u], %u, %u) OVERLAPS Square([%u, %u, %u], %u, %u): FALSE\n",
                                self->basicShape.vertex.x,
                                self->basicShape.vertex.y,
                                self->basicShape.vertex.z,
                                self->basicShape.width,
                                self->basicShape.length,
                                ((Square_t*)otherObject)->basicShape.vertex.x,
                                ((Square_t*)otherObject)->basicShape.vertex.y,
                                ((Square_t*)otherObject)->basicShape.vertex.z,
                                ((Square_t*)otherObject)->basicShape.width,
                                ((Square_t*)otherObject)->basicShape.length);
        }
        return ret;
    }

    case GEO_TYPE_POLYGON:
        return s_Polygon_overlaps(self, (Polygon_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE:
        return s_PolyhedralSurface_overlaps(self, (PolyhedralSurface_t*)otherObject);

    case GEO_TYPE_ELLIPSE_EXTRUSION:
        return s_EllipseExtrusion_overlaps(self, (EllipseExtrusion_t*)otherObject);

    case GEO_TYPE_SQUARE_EXTRUSION:
        return s_SquareExtrusion_overlaps(self, (SquareExtrusion_t*)otherObject);

    case GEO_TYPE_POLYGON_EXTRUSION:
        return s_PolygonExtrusion_overlaps(self, (PolygonExtrusion_t*)otherObject);

    case GEO_TYPE_POLYHEDRALSURFACE_EXTRUSION:
        return s_PolyhedralSurfaceExtrusion_overlaps(self, (PolyhedralSurfaceExtrusion_t*)otherObject);

    default:
        return false;
    }
}

Polygon_t*
Ellipse_asPolygon(Ellipse_t* self) {
    return NULL;
}

/******************************************************
 *          Static Functions Implementation
 ******************************************************/

static bool_t
s_Point_equals(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_disjoint(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_intersects(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_touches(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_crosses(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_within(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_contains(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_Point_overlaps(Ellipse_t* self, Point_t* otherObject) {
    return false;
}

static bool_t
s_LineString_equals(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_disjoint(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_intersects(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_touches(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_crosses(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_within(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_contains(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}

static bool_t
s_LineString_overlaps(Ellipse_t* self, LineString_t* otherObject) {
    return false;
}


static bool_t
s_Line_equals(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_disjoint(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_intersects(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_touches(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_crosses(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_within(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_contains(Ellipse_t* self, Line_t* otherObject) {
    return false;
}

static bool_t
s_Line_overlaps(Ellipse_t* self, Line_t* otherObject) {
    return false;
}


static bool_t
s_LineRing_equals(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_disjoint(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_intersects(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_touches(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_crosses(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_within(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_contains(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}

static bool_t
s_LineRing_overlaps(Ellipse_t* self, LineRing_t* otherObject) {
    return false;
}


static bool_t
s_Polygon_equals(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_disjoint(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_intersects(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_touches(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_crosses(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_within(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_contains(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}

static bool_t
s_Polygon_overlaps(Ellipse_t* self, Polygon_t* otherObject) {
    return false;
}


static bool_t
s_Ellipse_equals(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_disjoint(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_intersects(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_touches(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_crosses(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_within(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_contains(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}

static bool_t
s_Ellipse_overlaps(Ellipse_t* self, Ellipse_t* otherObject) {
    return false;
}


static bool_t
s_Square_equals(Ellipse_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_disjoint(Ellipse_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_intersects(Ellipse_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_touches(Ellipse_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_crosses(Ellipse_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_within(Ellipse_t* self, Square_t* otherObject) {
    assert(self);
    assert(otherObject);

    GeoMeasurement_t circleDistance_x = abs(self->basicShape.vertex.x - otherObject->basicShape.vertex.x);
    GeoMeasurement_t circleDistance_y = abs(self->basicShape.vertex.y - otherObject->basicShape.vertex.y);

    if (circleDistance_x > (otherObject->basicShape.width/2 + self->basicShape.width)) {
        return false;
    }
    if (circleDistance_y > (otherObject->basicShape.length/2 + self->basicShape.width)) {
        return false;
    }

    if (circleDistance_x <= (otherObject->basicShape.width/2)) {
        return true;
    }
    if (circleDistance_y <= (otherObject->basicShape.length/2)) {
        return true;
    }

    GeoMeasurement_t cornerDistance_x = (circleDistance_x - otherObject->basicShape.width/2) * (circleDistance_x - otherObject->basicShape.width/2);
    GeoMeasurement_t cornerDistance_y = (circleDistance_y - otherObject->basicShape.length/2) * (circleDistance_y - otherObject->basicShape.length/2);
    GeoMeasurement_t cornerDistance_sq = cornerDistance_x + cornerDistance_y;

    bool_t ret = (cornerDistance_sq <= (self->basicShape.width * self->basicShape.width));

    return ret;
}

static bool_t
s_Square_contains(Ellipse_t* self, Square_t* otherObject) {
    return false;
}

static bool_t
s_Square_overlaps(Ellipse_t* self, Square_t* otherObject) {
    assert(self);
    assert(otherObject);

    GeoMeasurement_t closestX;
    GeoMeasurement_t closestY;

    if (self->basicShape.vertex.x < otherObject->basicShape.vertex.x) {
        closestX = otherObject->basicShape.vertex.x;
    }
    else if (self->basicShape.vertex.x > (otherObject->basicShape.vertex.x + otherObject->basicShape.width)) {
        closestX = otherObject->basicShape.vertex.x + otherObject->basicShape.width;
    }
    else {
        closestX = self->basicShape.vertex.x;
    }

    if (self->basicShape.vertex.y < otherObject->basicShape.vertex.y) {
        closestY = otherObject->basicShape.vertex.y;
    }
    else if (self->basicShape.vertex.y > (otherObject->basicShape.vertex.y + otherObject->basicShape.length)) {
        closestY = otherObject->basicShape.vertex.y + otherObject->basicShape.length;
    }
    else {
        closestY = self->basicShape.vertex.y;
    }

    GeoMeasurement_t distX = abs(self->basicShape.vertex.x - closestX);
    GeoMeasurement_t distY = abs(self->basicShape.vertex.y - closestY);
    GeoMeasurement_t distance = (GeoMeasurement_t) sqrt((distX * distX) + (distY * distY));

    return distance < self->basicShape.width;
}


static bool_t
s_PolyhedralSurface_equals(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_disjoint(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_intersects(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_touches(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_crosses(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_within(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_contains(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurface_overlaps(Ellipse_t* self, PolyhedralSurface_t* otherObject) {
    return false;
}


static bool_t
s_EllipseExtrusion_equals(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_disjoint(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_intersects(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_touches(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_crosses(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_within(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_contains(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_EllipseExtrusion_overlaps(Ellipse_t* self, EllipseExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_SquareExtrusion_equals(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_disjoint(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_intersects(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_touches(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_crosses(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_within(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_contains(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_SquareExtrusion_overlaps(Ellipse_t* self, SquareExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_PolygonExtrusion_equals(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_disjoint(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_intersects(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_touches(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_crosses(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_within(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_contains(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolygonExtrusion_overlaps(Ellipse_t* self, PolygonExtrusion_t* otherObject) {
    return false;
}


static bool_t
s_PolyhedralSurfaceExtrusion_equals(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_disjoint(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_intersects(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_touches(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_crosses(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_within(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_contains(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

static bool_t
s_PolyhedralSurfaceExtrusion_overlaps(Ellipse_t* self, PolyhedralSurfaceExtrusion_t* otherObject) {
    return false;
}

