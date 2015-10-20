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


    // Applying all transformations to scene's objects
    apply_all_transformations(s);
    // Initializing screen coordinate members of vertices for all objects
    map_to_screen_coords(s, xres, yres);

    vector<char> grid(yres * xres);
    // Fill grid
    draw_objects(s, grid, xres, yres);
    // Create ppm from grid
    ppm p = create_ppm(xres, yres, grid);
    // Output ppm
    for (string line : p.lines)
        cout << line << endl;
}

