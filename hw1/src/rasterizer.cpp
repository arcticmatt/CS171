#include "rasterizer.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

/*
 * Outlines all objects in a scene.
 */
void draw_objects(scene *s, vector<char>& grid, int xres, int yres) {
    for (object *o : s->objects) {
        draw_object(o, grid, xres, yres);
    }
}

/*
 * Outlines all the faces of an object onto a grid using bresenham.
 */
void draw_object(object *o, vector<char>& grid, int xres, int yres) {
    for (face *f : o->faces) {
        // For each face, draw each line (if vertices are valid)
        float x1 = o->vertices[f->v1]->screen_x;
        float y1 = o->vertices[f->v1]->screen_y;
        float x2 = o->vertices[f->v2]->screen_x;
        float y2 = o->vertices[f->v2]->screen_y;
        float x3 = o->vertices[f->v3]->screen_x;
        float y3 = o->vertices[f->v3]->screen_y;
        // Draw line between v1 and v2
        if (x1 != -1 && x2 != -1)
            bresenham(x1, y1, x2, y2, grid, xres, yres);
        // Draw line between v2 and v3
        if (x2 != -1 && x3 != -1)
            bresenham(x2, y2, x3, y3, grid, xres, yres);
        // Draw line between v3 and v1
        if (x3 != -1 && x1 != -1)
            bresenham(x3, y3, x1, y1, grid, xres, yres);
    }
}

/*
 * This is the generalized Bresenham algorithm. It "draws" a line from
 * x0 to y0 by filling in the points on the line with 1s on the passed-in
 * grid array.
 *
 * We generalize the naive_first_octant_bresenham by making the following changes.
 * First, we determine if the abs(slope) > 1. If it is, then we want to flip
 * the axis we are iterating on. We also want to invert the slope, because since
 * we are iterating on a different axis, we are looking at the slope from a
 * different perspective.
 * That is, if abs(slope) > 1, we want to iterate on y values. Then, if the slope
 * is negative, then instead of incrementing x or y (the variable we are not
 * iterating on) by 1, we want to decrement.
 * Note that depending on the sign of (x1 - x0) and (y1 - y0), we'll either be
 * adding or subtracting values in different places. This is just to account for
 * moving with or against the slope.
 *
 * So basically, for large slopes, we do a "swap" of x and y, and for negative
 * slopes, we modify the sign of certain values.
 */
void bresenham(int x0, int y0, int x1, int y1, vector<char>& grid, int xres, int yres) {
    float error = 0;
    int change_val_y = (y0 < y1) ? 1 : -1;
    int change_val_x = (x0 < x1) ? 1 : -1;

    // Check if line is vertical
    if (x0 == x1) {
        int y = y0;
        int x = x0;
        while (y != y1) {
            fill_grid(x, y, xres, yres, grid);
            y = y + change_val_y;
        }
        fill_grid(x, y, xres, yres, grid);
        return;
    }

    //Check if line is horizontal
    if (y0 == y1) {
        int y = y0;
        int x = x0;
        while (x != x1) {
            fill_grid(x, y, xres, yres, grid);
            x = x + change_val_x;
        }
        fill_grid(x, y, xres, yres, grid);
        return;
    }

    float m = (float) (y1 - y0) / (x1 - x0);
    if (abs(m) > 1) { // Iterate on y
        int x = x0;
        // We are iterating on y, so invert the slope
        m = 1.0 / m;
        int y = y0;
        while (y != y1) {
            float sign_m = change_val_y * m;
            fill_grid(x, y, xres, yres, grid);
            if (abs(error + sign_m) <= 0.5) {
                error = error + sign_m;
            } else {
                error = error + sign_m - change_val_x;
                x = x + change_val_x;
            }
            y = y + change_val_y;
        }
        fill_grid(x, y, xres, yres, grid);
    } else { // Iterate on x
        int y = y0;
        int x = x0;
        while (x != x1) {
            float sign_m = change_val_x * m;
            fill_grid(x, y, xres, yres, grid);
            if (abs(error + sign_m) <= 0.5) {
                error = error + sign_m;
            } else {
                error = error + sign_m - change_val_y;
                y = y + change_val_y;
            }
            x = x + change_val_x;
        }
        fill_grid(x, y, xres, yres, grid);
    }
}

/*
 * Fill a point in the grid.
 */
void fill_grid(int x, int y, int xres, int yres, vector<char>& grid) {
    //cout << "Drawing point at (" << x << "," << y << ")" << endl;
    grid[(yres - y - 1) * xres + x] = 1;
}

/*
 * Creates a ppm struct with the given resolution, based on the passed-in
 * grid.
 */
ppm create_ppm(int xres, int yres, vector<char> grid) {
    vector<string> lines;
    string header("P3");
    lines.push_back(header);

    ostringstream res_string_stream;
    res_string_stream << xres << " " << yres;
    string res_line = res_string_stream.str();
    lines.push_back(res_line);

    string intensity("255");
    lines.push_back(intensity);

    // Equation of circle is (x - x_center)^2 + (y - y_center)^2 = r^2
    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            // Check if pixel is inside circle
            if (grid[y * xres + x] == 1) {
                lines.push_back(get_object_line());
            } else {
                lines.push_back(get_bg_line());
            }
        }
    }

    ppm p(xres, yres);
    p.lines = lines;
    return p;
}

/*
 * Returns a string representing a pixel in the circle.
 */
string get_object_line() {
    ostringstream color_string_stream;
    color_string_stream << OBJECT_R << " " << OBJECT_G << " " << OBJECT_B;
    string color_string = color_string_stream.str();
    return color_string;
}

/*
 * Returns a string representing a pixel in the background.
 */
string get_bg_line() {
    ostringstream color_string_stream;
    color_string_stream << BG_R << " " << BG_G << " " << BG_B;
    string color_string = color_string_stream.str();
    return color_string;
}
