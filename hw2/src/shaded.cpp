#include <stdio.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include "parser.h"
#include "transformer.h"

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    // Should be 4 arguments: the scene description file, xres, yres, and mode
    assert (argc == 5);

    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    int mode = atoi(argv[4]);

    cout << "SHADER: xres = " << xres << ", yres = " << yres << ", mode = " <<
        mode << endl;

    /* PART 1: Parse the scene description file */
    scene *s = parse_scene(infile);
    s->print();

    /*
     * PART 2: Create transformation matrices, and apply geometric
     * transformations to each object-copy's vertices.
     */
    s->pp_mat = get_perspective_projection_matrix(s);
    s->world_to_cam_mat = get_world_transform_matrix(s->position, s->orient);
    for (object *o : s->objects) {
        transform_object_geom(o);
    }

    /*
     * PART 3: For each object-copy, transform each of its surface normals
     * by the correct nromal transformation. Normalize normals after
     * transforming.
     */
    for (object *o : s->objects) {
        transform_object_normals(o);
        normalize_normals(o);
    }
    s->print();
}
