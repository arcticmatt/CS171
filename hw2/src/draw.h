#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "framework.h"

using namespace std;

struct ppm {
    vector<string> lines;
    int xres;
    int yres;
    ppm(int x, int y) : xres(x), yres(y) {}
};

/*******************************************************************************
 * Defines methods needed for drawing stuff
 ******************************************************************************/

Vector3f lighting(vertex *v, surface_normal *n, scene *s);

#endif
