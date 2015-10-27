#include <stdio.h>
#include <cfloat>
#include <iostream>
#include <cassert>
#include <fstream>
#include "parser.h"
#include "transformer.h"
#include "draw.h"

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    // Should be 4 arguments: the scene description file, xres, yres, and mode
    assert (argc == 5);

    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    int mode = atoi(argv[4]);

    //cout << "SHADER: xres = " << xres << ", yres = " << yres << ", mode = " <<
        //mode << endl;

    /* PART 1: Parse the scene description file */
    scene *s = parse_scene(infile);

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

    /*
     * PART 4: Implement the light algorithm. Don't forget attenuation.
     * Remember that the same vertex on two different faces can have a different
     * normal. So we will store the colors for vertices in the face struct.
     */
    /*
     * PART 5: Implement the algorithm for rasterizing colored triangles with
     * interpoloation via barycentric coordinates, backface culling, and
     * depth buffering.
     */
    MatrixColor grid(yres, xres);
    // Initialize colors to black
    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            grid(y, x) = color(0, 0, 0);
        }
    }
    MatrixXd depth_buffer(yres, xres);
    // Initialize all depths to very high value
    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            depth_buffer(y, x) = DBL_MAX;
        }
    }
    s->depth_buffer = depth_buffer;
    if (mode == 0) {
        for (object *o : s->objects) {
            for (face *f : o->faces) {
                // This takes care of calling the lighting algorithm and the
                // rasterization algorithm
                gouraud_shading(f, o, s, grid);
            }
        }
    }

    // Create ppm from grid and output it to stdout
    ppm p = create_ppm(xres, yres, grid);
    //for (string line : p.lines)
        //cout << line << endl;
}
