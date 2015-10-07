#include <stdio.h>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <sstream>
#include "ppm_gen.h"

/*
 * Creates a ppm struct with the given resolution. The ppm is as specified in
 * the assignment; that is, it is a PPM image of a colored circle centered on a
 * different colored background. The diameter of the circle is half of min(x, y).
 */
ppm create_ppm(int max_x, int max_y) {
    int diameter = min(max_x, max_y) / 2;
    int radius = diameter / 2;
    int x_center = max_x / 2;
    int y_center = max_y / 2;
    vector<string> lines;
    string header("P3");
    lines.push_back(header);

    ostringstream res_string_stream;
    res_string_stream << max_x << " " << max_y;
    string res_line = res_string_stream.str();
    lines.push_back(res_line);

    string intensity("255");
    lines.push_back(intensity);

    // Equation of circle is (x - x_center)^2 + (y - y_center)^2 = r^2
    for (int y = 0; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            // Check if pixel is inside circle
            if (pow((x - x_center), 2) + pow((y - y_center), 2) <= radius * radius) {
                lines.push_back(get_circle_line());
            } else {
                lines.push_back(get_bg_line());
            }
        }
    }

    ppm p(max_x, max_y);
    p.lines = lines;
    return p;
}

/*
 * Returns a string representing a pixel in the circle.
 */
string get_circle_line() {
    ostringstream color_string_stream;
    color_string_stream << CIRCLE_R << " " << " " << CIRCLE_G << " " << CIRCLE_B;
    string color_string = color_string_stream.str();
    return color_string;
}

/*
 * Returns a string representing a pixel in the background.
 */
string get_bg_line() {
    ostringstream color_string_stream;
    color_string_stream << BG_R << " " << " " << BG_G << " " << BG_B;
    string color_string = color_string_stream.str();
    return color_string;
}
