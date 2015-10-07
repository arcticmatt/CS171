#include <cassert>
#include <iostream>
#include <string>
#include "ppm_gen.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Should only be two arguments xres and yres
    assert (argc == 3);

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    // Get ppm struct
    ppm p = create_ppm(x, y);

    // Print the lines of the ppm
    for (string line : p.lines)
        cout << line << endl;
}
