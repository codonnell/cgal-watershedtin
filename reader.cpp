#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <CGAL/IO/Polyhedron_iostream.h>
#include <iostream>
#include <fstream>

#include <CGAL/bounding_box.h>
#include <CGAL/Timer.h>

#include <vector>
#include <iterator>
#include <algorithm>

#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Polyhedron_geomview_ostream.h>

#include "definitions.h"
#include "utils.h"

static bool DRAWING = false;

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " [input file]" << endl;
        std::abort();
    }

    Polyhedron P;
    std::ifstream input(argv[1]);
    assert(input);
    CGAL::Timer t;
    t.start();
    input >> P;
    std::transform(P.facets_begin(), P.facets_end(), P.planes_begin(), Plane_equation());
    t.stop();
    cout << "Input time: " << t.time() << endl;
    t.reset();

    t.start();
    label_all_edges(P);
    t.stop();
    cout << "Labelling time: " << t.time() << endl;
    t.reset();

    t.start();
    std::vector<Vertex> saddles;
    std::remove_copy_if(P.vertices_begin(), P.vertices_end(),
            std::back_inserter(saddles), is_not_saddle);
    t.stop();
    cout << "Saddle finding time: " << t.time() << endl;
    t.reset();

    if (DRAWING) {
        Kernel::Iso_cuboid_3 c =
            CGAL::bounding_box(P.points_begin(), P.points_end());
        cout << c << endl;
        CGAL::Geomview_stream gv(c.bbox());
        gv.set_wired(true);
        gv << P;
        cout << "Enter a key to finish" << endl;
        char ch;
        std::cin >> ch;
    }

    return 0;
}
