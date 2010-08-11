#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

/**
 * operator() calculates returns the plane for a given facet.
 */
struct Plane_equation {
    template <class Facet>
    typename Facet::Plane_3 operator()( Facet& f) {
        typename Facet::Halfedge_handle h = f.halfedge();
        typedef typename Facet::Plane_3  Plane;
        return Plane(h->vertex()->point(),
                     h->next()->vertex()->point(),
                     h->next()->next()->vertex()->point());
    }
};

/**
 * Determines whether the left facet of a halfedge slopes into it.
 *
 * If the halfedge is on the border, returns false. Otherwise, uses the normal
 * of the adjacent face to determine whether the face is sloping into or away
 * from the halfedge.
 */
bool slopes_into(const Halfedge_const_handle& h);

/**
 * Determines whether a plane is flat.
 */
bool is_flat_plane(const Plane_3& h);

/**
 * Determines whether h is a ridge.
 */
bool is_ridge(const Halfedge_const_handle& h);

/**
 * Determines whether h is a channel.
 */
bool is_channel(const Halfedge_const_handle& h);

/**
 * Determines whether h is transverse.
 */
bool is_transverse(const Halfedge_const_handle& h);

/**
 * Determines whether there is a generalized ridge up the face left of h.
 *
 * A generalized ridge is an upslope line through which water does not flow. A
 * generalized ridge can be found by determining whether water flows into both
 * edges adjacent to the point through which it runs. No generalized ridges run
 * through the infinity face.
 */
bool is_generalized_ridge(const Halfedge_const_handle& h);

/**
 * Determines whether there is a generalized channel up the face left of h.
 *
 * A generalized channel is a downslope line through which water does not flow. A
 * generalized channel can be found by determining whether water flows into both
 * edges adjacent to the point through which it runs. No generalized channels run
 * through the infinity face.
 */
bool is_generalized_channel(const Halfedge_const_handle& h);

#endif
