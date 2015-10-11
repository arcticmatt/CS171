#include <stdio.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include "parser.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Should be 3 arguments: the scene description file, xres, and yres
    assert (argc == 4);

    cout << "Input file = " << argv[1] << endl;
    ifstream infile(argv[1]);
    scene *s = parse_scene(infile);

    cout << "Printing the contents of the parsed scene object" << endl;
    s->print();
}
