#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

// template <class Refs, class T, class Point>
// struct My_vertex : CGAL::HalfedgeDS_vertex_base<Refs, T, Point> {
//     char type;
// };

enum EdgeType {
    NO_TYPE, // Type not yet calculated.
    IN, // Water flows into this halfedge
    OUT, // Water flows out from this halfedge
    RIDGE, // Water flows away on both sides.
    CHANNEL, // Water flows in on both sides.
    TRANSVERSE, // Water flows in on one side, away on the other.
    UPSLOPE, // Traced up generalized ridge.
    // Saddle edge types
    NEITHER, // Not closed on either side.
    LEFT, // Closed on the left.
    RIGHT, // Closed on the right.
    BOTH, // Closed on both sides.
    FLAT_CHAN // A flat channel.
};

template <class Refs>
struct Tin_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs> {
    unsigned int watershed;
    enum EdgeType type;
};

class Tin_Polyhedron_items_3 {
    public:
        template < class Refs, class Traits>
        struct Vertex_wrapper {
            typedef typename Traits::Point_3 Point;
            typedef CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point> Vertex;
            // typedef My_vertex<Refs, CGAL::Tag_true, Point> Vertex;
        };
        template < class Refs, class Traits>
        struct Halfedge_wrapper {
            // typedef CGAL::HalfedgeDS_halfedge_base<Refs> Halfedge;
            typedef Tin_halfedge<Refs> Halfedge;
        };
        template < class Refs, class Traits>
        struct Face_wrapper {
            typedef typename Traits::Plane_3 Plane;
            typedef CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true, Plane> Face;
        };
};


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel, Tin_Polyhedron_items_3> Polyhedron;

typedef Polyhedron::Halfedge_iterator Halfedge_iterator;
typedef Polyhedron::Halfedge_const_iterator Halfedge_const_iterator;
typedef Polyhedron::Vertex_iterator Vertex_iterator;
typedef Polyhedron::Vertex_const_iterator Vertex_const_iterator;
typedef Polyhedron::Facet_iterator Facet_iterator;
typedef Polyhedron::Facet_const_iterator Facet_const_iterator;
typedef Polyhedron::Point_iterator Point_iterator;
typedef Polyhedron::Plane_iterator Plane_iterator;

typedef Polyhedron::Halfedge Halfedge;
typedef Polyhedron::Vertex Vertex;
typedef Polyhedron::Facet Facet;

typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef Polyhedron::Halfedge_const_handle Halfedge_const_handle;
typedef Polyhedron::Vertex_handle Vertex_handle;
typedef Polyhedron::Vertex_const_handle Vertex_const_handle;
typedef Polyhedron::Facet_handle Facet_handle;
typedef Polyhedron::Facet_const_handle Facet_const_handle;

typedef Polyhedron::Plane_3 Plane_3;
typedef Polyhedron::Point_3 Point_3;

typedef Kernel::Vector_3 Vector_3;
typedef Kernel::Vector_2 Vector_2;
typedef Kernel::Point_2 Point_2;
typedef Kernel::Line_2 Line_2;
typedef Kernel::Ray_2 Ray_2;

#endif
