#ifndef __UTILS_H__
#define __UTILS_H__

#include "definitions.h"

enum TraceFlag {
    TRACE_CONTINUE,
    TRACE_POINT,
    TRACE_FINISH
};

/**
 * Calculates the edge type of a halfedge that has not already been typed.
 */
enum EdgeType edge_type(const Halfedge_const_handle& h);

/**
 * Determines whether v is not a saddle.
 */
bool is_not_saddle(const Vertex& v);

/**
 * Determines whether v is a saddle.
 *
 * A point is a saddle if it has a border halfedge coming from it or more than
 * one channel or ridge.
 */
bool is_saddle(const Vertex_const_handle& v);

/**
 * Finds the halfedge whose left face has the steepest slope.
 *
 * The returned halfedge or its left face must have the steepest slope around v.
 * This is exclusive of the next halfedge around the vertex.
 */
Halfedge_handle find_steepest_path(Vertex_handle v);

/**
 * Returns the exit point of the upslope path beginning at h's vertex.
 *
 * Updates flag to reflect whether a new vertex must be added to the graph
 * or if the exit point is at an existent vertex. Updates h so that it is the
 * halfedge on which the exit point is located.
 */
Point_3 find_upslope_intersection(Halfedge_handle& h, TraceFlag& flag);

#endif
