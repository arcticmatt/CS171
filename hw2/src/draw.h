#ifndef DRAW_H
#define DRAW_H

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

ppm create_ppm(int xres, int yres, MatrixColor grid);
string get_line(color c);

void gouraud_shading(face *f, object *o, scene *s, MatrixColor& grid);
color lighting(vertex *v, surface_normal *n, surface_material m, scene *s);
void raster_colored_triangle(vertex *a, vertex *b, vertex *c,
        color c_a, color c_b, color c_c, MatrixColor& grid, MatrixXd depth_buffer);
float compute_alpha(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c,
        int x, int y);
float compute_beta(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c,
        int x, int y);
float compute_gamma(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c,
        int x, int y);
float f(int x, int y, int x_i, int y_i, int x_j, int y_j);

#endif
