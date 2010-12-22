#include <cassert>

#include "definitions.h"
#include "primitives.h"
#include "utils.h"

static bool DEBUG_UTIL;

using std::cout;
using std::endl;

/**
 * Calculates the edge type of a halfedge that has not already been typed.
 */
enum EdgeType edge_type(const Halfedge_const_handle& h)
{
    assert(h->type == NO_TYPE);
    if (slopes_into(h))
        return IN;
    return OUT;
}

/**
 * Determines whether v is not a saddle.
 */
bool is_not_saddle(const Vertex& v)
{
    return !is_saddle(&v);
}

/**
 * Determines whether v is a saddle.
 *
 * A point is a saddle if it has a border halfedge coming from it or more than
 * one channel or ridge.
 */
bool is_saddle(const Vertex_const_handle& v)
{
    if (DEBUG_UTIL)
        print_neighborhood(*v);
    typedef Vertex::Halfedge_around_vertex_const_circulator Circulator;
    Circulator current = v->vertex_begin();
    Circulator end = v->vertex_begin();
    int count[2] = {0, 0}; // Tracks the number of ridges and channels
    do {
        if (current->is_border()) {
            if (DEBUG_UTIL) {
                cout << "Border edge:" << endl;
                print_halfedge(current);
            }
            return true;
        }
        if (DEBUG_UTIL)
            cout << "Normal: " << current->facet()->plane().orthogonal_vector()
                << endl;
        if (is_ridge(current))
            ++count[0];
        else if (is_channel(current))
            ++count[1];
        if (is_generalized_ridge(current))
            ++count[0];
        else if (is_generalized_channel(current))
            ++count[1];
    } while (++current != end);
    assert(count[0] == count[1]);
    return (count[0] > 1 || count[1] > 1);
}

/**
 * Finds the halfedge whose left face has the steepest slope.
 *
 * The returned halfedge or its left face must have the steepest slope around v.
 * This is exclusive of the next halfedge around the vertex.
 */
Halfedge_handle find_steepest_path(Vertex_handle v)
{
    typedef Vertex::Halfedge_around_vertex_circulator Circulator;
    Circulator current = v->vertex_begin();
    Circulator end = v->vertex_begin();
    Vector_3 steepest_vector = Vector_3(1, 0, 0);
    Halfedge_handle steepest_halfedge = current;
    do {
        Vector_3 normal;
        // The steepest path must be an upslope ridge or a generalized ridge.
        if (is_ridge(current) &&
                current->opposite()->vertex()->point().z() > v->point().z())
            normal = Vector_3(v->point(), current->opposite()->vertex()->point());
        else if (is_generalized_ridge(current)) {
            Vector_3 perp = current->facet()->plane().orthogonal_vector();
            normal = Vector_3(perp.x(), perp.y(), 1 / perp.z());
        }
        else
            continue;
        if (is_steeper(normal, steepest_vector)) {
            steepest_vector = normal;
            steepest_halfedge = current;
        }
    } while (++current != end);
    DEBUG_UTIL = true;
    if (DEBUG_UTIL) {
        print_neighborhood(*v);
        cout << "Steepest vector: " << steepest_vector << endl;
        cout << "Steepest halfedge: " << endl;
        print_halfedge(steepest_halfedge);
    }
    DEBUG_UTIL = false;
    return steepest_halfedge;
}

/**
 * Returns the exit point of the upslope path beginning at h's vertex.
 *
 * Updates flag to reflect whether a new vertex must be added to the graph
 * or if the exit point is at an existent vertex. Updates h so that it is the
 * halfedge on which the exit point is located.
 */
Point_3 find_upslope_intersection(Halfedge_handle& h, TraceFlag& flag)
{
    Vector_3 normal_3 = h->facet()->plane().orthogonal_vector();
    // We need the upslope, not downslope path, so we negate x and y vals.
    Vector_2 normal_2 = Vector_2(-normal_3.x(), -normal_3.y());
    Point_2 start_point = Point_2(h->vertex()->point().x(),
            h->vertex()->point().y());
    Ray_2 upslope_path = Ray_2(start_point, normal_2);

    Point_2 exit_2 = find_exit(h, upslope_path, start_point);
    Point_3 exit_3 = h->facet()->plane().to_3d(exit_2);
    if (exit_3 == h->vertex()->point())
        flag = TRACE_POINT;
    else
        flag = TRACE_CONTINUE;
    return exit_3;
}
