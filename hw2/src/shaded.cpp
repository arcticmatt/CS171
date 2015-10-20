#include <stdio.h>
#include <iostream>
#include <cassert>
#include <fstream>

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    // Should be 4 arguments: the scene description file, xres, yres, and mode
    assert (argc == 5);

    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);
    int mode = atoi(argv[4]);

    cout << "xres = " << xres << ", yres = " << yres << ", mode = " << mode << endl;
}

