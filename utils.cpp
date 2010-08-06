#include <cassert>

#include "definitions.h"
#include "utils.h"

extern bool DEBUG;

using std::cout;
using std::endl;

/**
 * Determines whether the left facet of a halfedge slopes into it.
 *
 * If the halfedge is on the border, returns false. Otherwise, uses the normal
 * of the adjacent face to determine whether the face is sloping into or away
 * from the halfedge.
 */
bool slopes_into(const Halfedge_const_handle& h)
{
    if (h->is_border())
        return false;
    Vector_3 normal;
    if (is_flat_plane(h->facet()->plane())) {
        normal = Vector_3(-1.0, 0.0, 0.0);
    }
    else
        normal = h->facet()->plane().orthogonal_vector();
    // Origin of h
    const Point_3 origin_3 = h->vertex()->point();
    const Point_2 origin_2 = Point_2(origin_3.x(), origin_3.y());
    // Dest of h
    const Point_3 dest_3 = h->next()->vertex()->point();
    const Point_2 dest_2 = Point_2(dest_3.x(), dest_3.y());
    // Displacement by flow direction of h
    const Point_3 disp_point_3 = origin_3 + normal;
    const Point_2 disp_point_2 = Point_2(disp_point_3.x(), disp_point_3.y());

    CGAL::Orientation o = orientation(origin_2, dest_2, disp_point_2);
    if (o == CGAL::RIGHT_TURN)
        return false;
    return true; // o == LEFT_TURN or COLLINEAR
}

/**
 * Determines whether a plane is flat.
 */
bool is_flat_plane(const Plane_3& h)
{
    return h.a() == 0.0 && h.b() == 0.0;
}

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
    if (is_ridge(h))
        return RIDGE;
    else if (is_channel(h))
        return CHANNEL;
    assert(is_transverse(h));
    return TRANSVERSE;
}

/**
 * Determines whether h is a ridge.
 */
bool is_ridge(const Halfedge_const_handle& h)
{
    if (h->type != NO_TYPE)
        return h->type == RIDGE;
    bool ret_val = !(slopes_into(h) || slopes_into(h->opposite()));
    if (DEBUG) {
        cout << (ret_val ? "" : "Not ") << "Ridge:" << endl;
        print_halfedge(h);
    }
    return ret_val;
}

/**
 * Determines whether h is a channel.
 */
bool is_channel(const Halfedge_const_handle& h)
{
    if (h->type != NO_TYPE)
        return h->type == CHANNEL;
    bool ret_val = slopes_into(h) && slopes_into(h->opposite());
    if (DEBUG) {
        cout << (ret_val ? "" : "Not ") << "Channel:" << endl;
        print_halfedge(h);
    }
    return ret_val;
}

/**
 * Determines whether h is transverse.
 */
bool is_transverse(const Halfedge_const_handle& h)
{
    if (h->type != NO_TYPE)
        return h->type == TRANSVERSE;
    bool ret_val = ((slopes_into(h) && !slopes_into(h->opposite())) ||
            (!slopes_into(h) && slopes_into(h->opposite())));
    if (DEBUG) {
        cout << (ret_val ? "" : "Not ") << "Transverse:" << endl;
        print_halfedge(h);
    }
    return ret_val;
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
    DEBUG = true;
    if (DEBUG)
        print_neighborhood(*v);
    typedef Vertex::Halfedge_around_vertex_const_circulator Circulator;
    Circulator start = v->vertex_begin();
    Circulator end = v->vertex_begin();
    int count[2] = {0, 0}; // Tracks the number of ridges and channels
    do {
        if (start->is_border()) {
            if (DEBUG) {
                cout << "Border edge:" << endl;
                print_halfedge(start);
            }
            return true;
        }
        if (DEBUG)
            cout << "Normal: " << start->facet()->plane().orthogonal_vector() << endl;
        if (is_ridge(start)) {
            if (DEBUG) {
                cout << "Ridge:" << endl;
                print_halfedge(start);
            }
            ++count[0];
        }
        else if (is_channel(start)) {
            if (DEBUG) {
                cout << "Channel:" << endl;
                print_halfedge(start);
            }
            ++count[1];
        }
        if (is_generalized_ridge(start)) {
            if (DEBUG) {
                cout << "Generalized Ridge:" << endl;
                print_halfedge(start);
                Circulator temp = start;
                print_halfedge(++temp);
            }
            ++count[0];
        }
        else if (is_generalized_channel(start)) {
            if (DEBUG) {
                cout << "Generalized Channel:" << endl;
                print_halfedge(start);
                Circulator temp = start;
                print_halfedge(++temp);
            }
            ++count[1];
        }
    } while (++start != end);
    DEBUG = false;
    assert(count[0] == count[1]);
    return (count[0] > 1 || count[1] > 1);
}

/**
 * Determines whether there is a generalized ridge up the face right of h.
 *
 * A generalized ridge is an upslope line through which water does not flow. A
 * generalized ridge can be found by determining whether water flows into both
 * edges adjacent to the point through which it runs. No generalized ridges run
 * through the infinity face.
 */
bool is_generalized_ridge(const Halfedge_const_handle& h)
{
    const Halfedge_const_handle e = h->opposite();
    if (e->is_border() && e->next()->is_border())
        return false;
    return slopes_into(e) && slopes_into(e->next());
}

/**
 * Determines whether there is a generalized channel up the face right of h.
 *
 * A generalized channel is a downslope line through which water does not flow. A
 * generalized channel can be found by determining whether water flows into both
 * edges adjacent to the point through which it runs. No generalized channels run
 * through the infinity face.
 */
bool is_generalized_channel(const Halfedge_const_handle& h)
{
    const Halfedge_const_handle e = h->opposite();
    if (e->is_border() && e->next()->is_border())
        return false;
    return !(slopes_into(e) || slopes_into(e->next()));
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
        cout << start->next()->vertex()->point() << endl;
    } while (++start != end);
    cout << endl;
}


/**
 * Prints the two points of a halfedge.
 */
void print_halfedge(const Halfedge_const_handle& h)
{
    cout << h->vertex()->point() << endl;
    cout << h->next()->vertex()->point() << endl;
}
