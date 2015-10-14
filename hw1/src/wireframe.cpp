#include <stdio.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include "rasterizer.h"
#include "transformer.h"

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    // Should be 3 arguments: the scene description file, xres, and yres
    assert (argc == 4);

    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    scene *s = parse_scene(infile);

    //cout << "Printing the contents of the parsed scene object" << endl;
    //s->print();

    //cout << "Printing world transform matrix" << endl;
    MatrixXd world_transf_mat = get_world_transform_matrix(s->position, s->orient);
    //cout << world_transf_mat << endl;

    //cout << "Printing perspective projection matrix" << endl;
    MatrixXd persp_proj_mat = get_perspective_projection_matrix(s);
    //cout << persp_proj_mat << endl;
    //s->print();

    //cout << "Applying all transformations to scene's objects (geom, camera, NDC)" << endl;
    apply_all_transformations(s);
    //s->print();

    //cout << "Initializing screen coordinate members of vertices for all objects" << endl;
    map_to_screen_coords(s, xres, yres);
    //s->print();

    //s->print();
    vector<char> grid(yres * xres);
    // Fill grid
    draw_objects(s, grid, xres, yres);
    // Create ppm from grid
    ppm p = create_ppm(xres, yres, grid);
    // Output ppm
    for (string line : p.lines)
        cout << line << endl;
}

