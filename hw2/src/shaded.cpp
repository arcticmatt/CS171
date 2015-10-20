#include <stdio.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include "parser.h"

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

    scene *s = parse_scene(infile);
    s->print();
}
