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
    
    if (DEBUG_PRIM) {
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
    if (DEBUG_PRIM) {
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
    if (DEBUG_PRIM) {
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
    if (DEBUG_PRIM) {
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
    if (DEBUG_PRIM) {
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
    if (DEBUG_PRIM) {
        cout << (ret_val ? "" : "Not ") << "Generalized Channel:" << endl;
        print_halfedge(h);
        print_halfedge(h->next());
    }
    return ret_val;
}

/**
 * True if u is steeper than v. Uses the square of slope to avoid sqrt.
 */
bool is_steeper(Vector_3 u, Vector_3 v)
{
    Vector_2 u_2 = Vector_2(u.x(), u.y());
    Vector_2 v_2 = Vector_2(v.x(), v.y());
    return ((u.z() * u.z() / u_2.squared_length()) > 
            (v.z() * v.z() / v_2.squared_length())); 
}

/**
 * Finds the exit point of upslope_path on the facet left of h.
 *
 * upslope_path must intersect the boundary of the facet in 2 points or a
 * segment. One of these points must be start_point. If the intersection is a
 * segment, returns the endpoint that is not start_point. Otherwise returns the
 * other intersection point.
 */
Point_2 find_exit(Halfedge_handle& h, Ray_2 upslope_path, Point_2 start_point)
{
    Point_2 exit;
    Plane_3 plane = h->facet()->plane();
    typedef Facet::Halfedge_around_facet_const_circulator Circulator;
    Circulator current = h->facet()->facet_begin();
    Circulator end = h->facet()->facet_begin();

    do {
        Point_2 source = plane.to_2d(current->vertex()->point());
        Point_2 target = plane.to_2d(current->opposite()->vertex()->point());
        Segment_2 seg = Segment_2(source, target);
        CGAL::Object intersect = CGAL::intersection(upslope_path, seg);
        // handle the point intersection case with *ipoint.
        if (const CGAL::Point_2<Kernel> *ipoint =
                CGAL::object_cast<CGAL::Point_2<Kernel> >(&intersect)) {
            if (*ipoint != start_point)
                return *ipoint;
        } 
        // handle the segment intersection case with *iseg.
        else if (const CGAL::Segment_2<Kernel> *iseg =
                CGAL::object_cast<CGAL::Segment_2<Kernel> >(&intersect)) {
            if (iseg->source() == start_point)
                return iseg->target();
            return iseg->source();
        } 
    } while (++current != end);
    cout << "Start: " << start_point << endl;
    cout << "Upslope path: " << upslope_path << endl;
    print_facet(*h->facet());
    std::abort();
    return exit;
}

/**
 * Prints all points adjacent to the input vertex.
 */
void print_neighborhood(const Vertex& v)
{
    cout << "Printing points around " << v.point() << endl;
    typedef Vertex::Halfedge_around_vertex_const_circulator Circulator;
    Circulator current = v.vertex_begin();
    Circulator end = v.vertex_begin();
    do {
        cout << current->opposite()->vertex()->point() << endl;
    } while (++current != end);
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

/**
 * Prints the points around a facet.
 */
void print_facet(const Facet& f)
{
    typedef Vertex::Halfedge_around_facet_const_circulator Circulator;
    Circulator current = f.facet_begin();
    Circulator end = f.facet_begin();
    do {
        cout << current->vertex()->point() << endl;
    } while (++current != end);
    cout << endl;
}
