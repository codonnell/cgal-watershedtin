#ifndef __WATERSHED_H__
#define __WATERSHED_H__

#include "definitions.h"

/**
 * Set the label on all edges to be CHANNEL, RIDGE, or TRANSVERSE.
 */
void label_all_edges(Polyhedron& p);

/**
 * Trace all upslope paths from a saddle vertex.
 *
 * Creates edges in the graph along all steepest paths up from the vertex,
 * tracing them until they reach a saddle or a ridge.
 */
void trace_from_saddle(Vertex& v);

/**
 * Trace up from this edge's vertex along the face to its left.
 */
void trace_up(Halfedge_handle& h);

/**
 * Trace up one face and modify h to be ready for the next trace.
 *
 * h must have the next face to be traced on its left, and its point must be the
 * next point to be traced from.
 */
void trace_up_once(Halfedge_handle& h, TraceFlag& flag);

/**
 * Determine whether a traceup has finished.
 *
 * A traceup is finished when it reaches a saddle point, a ridge, or a border.
 */
bool trace_finished(const Halfedge_const_handle& h, const TraceFlag& flag);

#endif
