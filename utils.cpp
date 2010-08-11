#include <cassert>

#include "definitions.h"
#include "primitives.h"
#include "utils.h"

static bool DEBUG_UTIL;

using std::cout;
using std::endl;

/**
 * Set the label on all edges to be CHANNEL, RIDGE, or TRANSVERSE.
 */
void label_all_edges(Polyhedron& p)
{
    for (Halfedge_iterator i = p.halfedges_begin(); i != p.halfedges_end(); ++i) {
        // type is not initialized by the constructor, so we initialize it here.
        i->type = NO_TYPE;
        i->type = edge_type(i);
    }
}

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
    Circulator start = v->vertex_begin();
    Circulator end = v->vertex_begin();
    int count[2] = {0, 0}; // Tracks the number of ridges and channels
    do {
        if (start->is_border()) {
            if (DEBUG_UTIL) {
                cout << "Border edge:" << endl;
                print_halfedge(start);
            }
            return true;
        }
        if (DEBUG_UTIL)
            cout << "Normal: " << start->facet()->plane().orthogonal_vector() <<
                endl;
        if (is_ridge(start))
            ++count[0];
        else if (is_channel(start))
            ++count[1];
        if (is_generalized_ridge(start))
            ++count[0];
        else if (is_generalized_channel(start))
            ++count[1];
    } while (++start != end);
    assert(count[0] == count[1]);
    return (count[0] > 1 || count[1] > 1);
}

/**
 * Prints all points adjacent to the input vertex.
 */
void print_neighborhood(const Vertex& v)
{
    cout << "Printing points around " << v.point() << endl;
    typedef Vertex::Halfedge_around_vertex_const_circulator Circulator;
    Circulator start = v.vertex_begin();
    Circulator end = v.vertex_begin();
    do {
        cout << start->opposite()->vertex()->point() << endl;
    } while (++start != end);
    cout << endl;
}


/**
 * Prints the two points of a halfedge.
 */
void print_halfedge(const Halfedge_const_handle& h)
{
    cout << h->opposite()->vertex()->point() << endl;
    cout << h->vertex()->point() << endl;
}
