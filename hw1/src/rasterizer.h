#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "framework.h"

using namespace std;

const int OBJECT_R = 100;
const int OBJECT_G = 23;
const int OBJECT_B = 232;

const int BG_R = 45;
const int BG_G = 18;
const int BG_B = 10;

struct ppm {
    vector<string> lines;
    int xres;
    int yres;
    ppm(int x, int y) : xres(x), yres(y) {}
};

ppm create_ppm(int xres, int yres, vector<char> grid);
string get_object_line();
string get_bg_line();
void draw_objects(scene *s, vector<char>& grid, int xres, int yres);
void draw_object(object *o, vector<char>& grid, int xres, int yres);
void bresenham(int x0, int y0, int x1, int y1, vector<char>& grid,
        int xres, int yres);
void fill_grid(int x, int y, int xres, int yres, vector<char>& grid);

#endif
