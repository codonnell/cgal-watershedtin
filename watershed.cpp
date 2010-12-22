#include <cassert>

#include "definitions.h"
#include "primitives.h"
#include "utils.h"
#include "watershed.h"

using std::cout;
using std::endl;

/**
 * Set the label on all edges to be CHANNEL, RIDGE, or TRANSVERSE.
 */
void label_all_edges(Polyhedron& p)
{
    for (Halfedge_iterator i = p.halfedges_begin(); i != p.halfedges_end(); ++i)
    {
        // type is not initialized by the constructor, so we initialize it here.
        i->type = NO_TYPE;
        i->type = edge_type(i);
    }
}

/**
 * Trace all upslope paths from a saddle vertex.
 *
 * Creates edges in the graph along all steepest paths up from the vertex,
 * tracing them until they reach a saddle or a ridge.
 */
void trace_from_saddle(Vertex& v)
{
    assert(is_saddle(&v));
    typedef Vertex::Halfedge_around_vertex_circulator Circulator;
    Circulator start = v.vertex_begin();
    Circulator end = v.vertex_begin();
    do {
        trace_up(start);
    } while (++start != end);
}

/**
 * Trace up from this edge's vertex along the face to its left and onward.
 */
void trace_up(Halfedge_handle& h)
{
    enum TraceFlag flag = TRACE_CONTINUE;
    do {
        trace_up_once(h, flag);
    } while (!trace_finished(h, flag));
}

/**
 * Trace up one face and modify h to be ready for the next trace.
 *
 * h must have the next face to be traced on its left, and its point must be the
 * next point to be traced from.
 */
void trace_up_once(Halfedge_handle& h, TraceFlag& flag)
{
    if (flag == TRACE_POINT) {
        assert(!is_saddle(h->vertex()));
        h = find_steepest_path(h->vertex());
    }
    Point_3 intersect_point = find_upslope_intersection(h, flag);
}

/**
 * Determine whether a traceup has finished.
 *
 * A traceup is finished when it reaches a saddle point, a ridge, or a border.
 */
bool trace_finished(const Halfedge_const_handle& h, const TraceFlag& flag)
{
    return (is_saddle(h->vertex()) || is_ridge(h) || h->is_border());
}
