#include <cassert>

#include "definitions.h"
#include "primitives.h"

static bool DEBUG_PRIM = false;

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
    if (h->type == IN)
        return true;
    else if (h->type == OUT)
        return false;

    if (h->is_border())
        return false;
    Vector_3 normal;
    if (is_flat_plane(h->facet()->plane()))
        normal = Vector_3(-1.0, 0.0, 0.0);
    else
        normal = h->facet()->plane().orthogonal_vector();
    // Origin of h
    const Point_3 origin_3 = h->opposite()->vertex()->point();
    const Point_2 origin_2 = Point_2(origin_3.x(), origin_3.y());
    // Dest of h
    const Point_3 dest_3 = h->vertex()->point();
    const Point_2 dest_2 = Point_2(dest_3.x(), dest_3.y());
    // Displacement by flow direction of h
    const Point_3 disp_point_3 = origin_3 + normal;
    const Point_2 disp_point_2 = Point_2(disp_point_3.x(), disp_point_3.y());
    
    if (DEBUG_UTIL) {
        cout << "Normal: " << normal << endl;
        cout << "Origin: " << origin_3 << endl;
        cout << "Dest: " << dest_3 << endl;
        cout << "Disp: " << disp_point_3 << endl;
    }

    CGAL::Orientation o = orientation(origin_2, dest_2, disp_point_2);
    return (o == CGAL::RIGHT_TURN);
}

/**
 * Determines whether a plane is flat.
 */
bool is_flat_plane(const Plane_3& h)
{
    return h.a() == 0.0 && h.b() == 0.0;
}

/**
 * Determines whether h is a ridge.
 */
bool is_ridge(const Halfedge_const_handle& h)
{
    bool ret_val = !(slopes_into(h) || slopes_into(h->opposite()));
    if (DEBUG_UTIL) {
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
    bool ret_val = slopes_into(h) && slopes_into(h->opposite());
    if (DEBUG_UTIL) {
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
    bool ret_val = ((slopes_into(h) && !slopes_into(h->opposite())) ||
            (!slopes_into(h) && slopes_into(h->opposite())));
    if (DEBUG_UTIL) {
        cout << (ret_val ? "" : "Not ") << "Transverse:" << endl;
        print_halfedge(h);
    }
    return ret_val;
}

/**
 * Is there a generalized ridge up the face left of h starting at h's vertex?
 *
 * A generalized ridge is an upslope line through which water does not flow. A
 * generalized ridge can be found by determining whether water flows into both
 * edges adjacent to the point through which it runs. No generalized ridges run
 * through the infinity face.
 */
bool is_generalized_ridge(const Halfedge_const_handle& h)
{
    bool ret_val;
    if (h->is_border()) {
        assert(h->next()->is_border());
        ret_val = false;
    }
    else {
       ret_val = (slopes_into(h) && slopes_into(h->next()));
    }
    if (DEBUG_UTIL) {
        cout << (ret_val ? "" : "Not ") << "Generalized Ridge:" << endl;
        print_halfedge(h);
        print_halfedge(h->next());
    }
    return ret_val;
}

/**
 * Is there a generalized channel up the face left of h starting at h's vertex?
 *
 * A generalized channel is a downslope line through which water does not flow. A
 * generalized channel can be found by determining whether water flows into both
 * edges adjacent to the point through which it runs. No generalized channels run
 * through the infinity face.
 */
bool is_generalized_channel(const Halfedge_const_handle& h)
{
    bool ret_val;
    if (h->is_border()) {
        assert(h->next()->is_border());
        ret_val = false;
    }
    else {
       ret_val = !(slopes_into(h) || slopes_into(h->next()));
    }
    if (DEBUG_UTIL) {
        cout << (ret_val ? "" : "Not ") << "Generalized Channel:" << endl;
        print_halfedge(h);
        print_halfedge(h->next());
    }
    return ret_val;
}
