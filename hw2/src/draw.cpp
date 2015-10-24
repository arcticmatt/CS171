#include "draw.h"
#include "transformer.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

/*
 * This is the Gouraud shading algorithm. The idea is that for each face/triangle
 * of our object, we use the lighting model to calculate the illuminated color
 * at each vertex, and then use barycentric interpolation to rasterize the triangle.
 * This lends itself to Gouraud shading's other name, per vertex lighting.
 */
void gouraud_shading(face *f, object *o, scene *s, MatrixColor& grid) {
    vertex *a = o->vertices[f->v1];
    vertex *b = o->vertices[f->v2];
    vertex *c = o->vertices[f->v3];

    //cout << "lighting..." << endl;

    //cout << "lighting for vertex #" << f->v1 << ", normal #" << f->vn1 << endl;
    f->c1 = lighting(a, o->normals[f->vn1], o->material, s);
    //cout << "lighting for vertex #" << f->v2 << ", normal #" << f->vn2 << endl;
    f->c2 = lighting(b, o->normals[f->vn2], o->material, s);
    //cout << "lighting for vertex #" << f->v3 << ", normal #" << f->vn3 << endl;
    f->c3 = lighting(c, o->normals[f->vn3], o->material, s);

    //cout << "converting world -> ndc..." << endl;

    world_to_ndc(a, s);
    world_to_ndc(b, s);
    world_to_ndc(c, s);

    //cout << "rasterizing colored triangles..." << endl;

    raster_colored_triangle(a, b, c, f->c1, f->c2, f->c3, grid, s->depth_buffer);

    //cout << "Done with gouaraud shading!" << endl;
}

/*
 * This is the algorithm for the lighting model, which computes the color
 * of a point v on an illuminated surface. It takes in:
 * v: The position of the point v as a triple of (x, y, z) coordinates.
 * n: The surface normal as a triple of (x, y, z) coordinates.
 * m: The material reflectance and shininess properties.
 * s: The scene, which contains the list of light sources and the camera
 *    position.
 *
 * Note that in this function, we convert our structs (vertex, color,
 * surface_normal) to Eigen Vectors to make the vector math easier to perform.
 */
color lighting(vertex *v, surface_normal *n, surface_material m, scene *s) {
    //cout << "================= LIGHTING =================" << endl;
    Vector3f col_diff = m.diffuse.get_vec();
    Vector3f col_amb = m.ambient.get_vec();
    Vector3f col_spec = m.specular.get_vec();
    float shine = m.shininess;

    Vector3f diff_sum(3, 1);
    diff_sum << 0, 0, 0;
    Vector3f spec_sum(3, 1);
    spec_sum << 0, 0, 0;

    Vector3f point_pos = v->get_vec();
    Vector3f cam_pos = s->position.get_vec();
    Vector3f cam_dir = cam_pos - point_pos;
    cam_dir.normalize();

    Vector3f normal_vec = n->get_vec();

    float zero = 0;
    for (light *l : s->lights) {
        Vector3f light_pos = l->position.get_vec();
        Vector3f light_col = l->colr.get_vec();
        Vector3f light_dist = light_pos - point_pos;
        Vector3f light_dir = light_dir.normalized();

        // Factor in attenutation
        float light_distance = light_dist.norm();
        float attenuation_const = 1.0 /
            (1.0 + l->attenuation * light_distance * light_distance);
        light_col *= attenuation_const; // Scalar product
        //cout << "light_color (after attenuation) = (" <<
            //light_col(0,0) << "," << light_col(1,0) << "," << light_col(2,0) <<
            //")" << endl;

        // Scalar prod
        float dot_prod_diff = normal_vec.dot(light_dir);
        Vector3f light_diff = light_col * max(zero, dot_prod_diff);
        //cout << "light_diff = (" <<
            //light_diff(0,0) << "," << light_diff(1,0) << "," << light_diff(2,0) <<
            //")" << endl;
        diff_sum += light_diff;

        Vector3f h = (cam_dir + light_dir).normalized();
        float dot_prod_spec = normal_vec.dot(h);
        Vector3f light_spec = light_col * pow(max(zero, dot_prod_spec), shine);
        //cout << "light_spec = (" <<
            //light_spec(0,0) << "," << light_spec(1,0) << "," << light_spec(2,0) <<
            //")" << endl;
        spec_sum += light_spec;
    }

    Vector3f ones(1.0, 1.0, 1.0);
    Vector3f col = col_amb + diff_sum.cwiseProduct(col_diff) +
        spec_sum.cwiseProduct(col_spec);
    Vector3f min_color = ones.cwiseMin(col);
    color final_color(min_color(0, 0), min_color(1, 0), min_color(2, 0));
    //cout << "================= END =================" << endl;
    return final_color;
}

/*
 * This is the algorithm for rasterizing colored triangles with interpolation
 * via barycentric coordinates, backface culling, and depth buffering.
 * It takes in:
 * a: The first point of a face (NDC coordinates).
 * b: The second point of a face (NDC coordinates).
 * c: The third point of a face (NDC coordinates).
 * grid: The grid to fill with colors.
 */
void raster_colored_triangle(vertex *a, vertex *b, vertex *c, color c_a,
        color c_b, color c_c, MatrixColor& grid, MatrixXd depth_buffer) {
    int xres = grid.cols();
    int yres = grid.rows();

    Vector3f ndc_a = a->get_vec();
    Vector3f ndc_b = b->get_vec();
    Vector3f ndc_c = c->get_vec();

    // Perform backface culling, ignoring faces facing away from the camera
    Vector3f cross = (ndc_c - ndc_b).cross(ndc_a - ndc_b);
    if (cross(2, 0) < 0)
        return;
    map_to_screen_coords(a, xres, yres);
    map_to_screen_coords(b, xres, yres);
    map_to_screen_coords(c, xres, yres);

    // TODO: what if mapping to screen coords determined vertex was out of
    // bounds, so the screen coords stay at -1?
    int x_a = a->screen_x;
    int y_a = a->screen_y;
    int x_b = b->screen_x;
    int y_b = b->screen_y;
    int x_c = c->screen_x;
    int y_c = c->screen_y;

    int x_min = min({x_a, x_b, x_c});
    int x_max = max({x_a, x_b, x_c});
    int y_min = min({y_a, y_b, y_c});
    int y_max = max({y_a, y_b, y_c});

    //cout << "x_min = " << x_min << " and y_min = " << y_min << endl;
    //cout << "x_max = " << x_max << " and y_max = " << y_max << endl;
    for (int x = x_min; x < x_max; x++) {
        for (int y = y_min; y < y_max; y++) {
            //cout << "iteration (x, y) = ( " << x << "," << y << ")" << endl;
            //cout << "x_a = " << x_a << ", y_a = " << y_a << ", x_b = " <<
                //x_b << ", y_b = " << y_b << ", x_c = " << x_c <<
                //", y_c = " << y_c << endl;

            float alpha = compute_alpha(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
            float beta = compute_beta(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
            float gamma = compute_gamma(x_a, y_a, x_b, y_b, x_c, y_c, x, y);
            //cout << "alpha = " << alpha << ", beta = " << beta <<
                //", gamma = " << gamma << endl;

            // Check to make sure the point is inside the triangle
            if ((alpha > 0 && alpha < 1) && (beta > 0 && beta < 1)
                    && (gamma > 0 && gamma < 1)) {
                Vector3f ndc = alpha * ndc_a + beta * ndc_b + gamma * ndc_c;
                // Check to see if NDC coords are in cube
                bool in_cube = abs(ndc(0, 0)) < 1 && abs(ndc(1, 0)) < 1
                    && abs(ndc(2, 0)) < 1;
                // TODO: is indexing of depth_buffer right
                //cout << "in_cube = " << in_cube << endl;
                cout.flush();
                if (in_cube && ndc(2, 0) < depth_buffer(y, x)) {
                    //cout << "drawing at (y,x) = (" << y << "," << x << ")" << endl;
                    depth_buffer(y, x) = ndc(2, 0);
                    float r = alpha * c_a.r + beta * c_b.r + gamma * c_c.r;
                    float g = alpha * c_a.g + beta * c_b.g + gamma * c_c.g;
                    float b = alpha * c_a.b + beta * c_b.b + gamma * c_c.b;
                    //cout << "c_a = (" << c_a.r << "," << c_a.g << "," <<
                        //c_a.b << ")" << endl;
                    //cout << "c_b = (" << c_b.r << "," << c_b.g << "," <<
                        //c_b.b << ")" << endl;
                    //cout << "c_c = (" << c_c.r << "," << c_c.g << "," <<
                        //c_c.b << ")" << endl;
                    //cout << "alpha = " << alpha << ", beta = " << beta <<
                        //", gamma = " << gamma << endl;
                    //cout << "color = (" << r << "," << g << "," << b << ")"
                        //<< endl;
                    color c(r, g, b);
                    grid(y, x) = c;
                }
            }
        }
    }

    //cout << "Exiting rasterization function" << endl;
}

/*
 * Calculates alpha, which is used to compute barycentric coordinates.
 */
float compute_alpha(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c,
        int x, int y) {
    float numerator = f(x, y, x_b, y_b, x_c, y_c);
    float denom = f(x_a, y_a, x_b, y_b, x_c, y_c);
    return numerator / denom;
}

/*
 * Calculates beta, which is used to compute barycentric coordinates.
 */
float compute_beta(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c,
        int x, int y) {
    float numerator = f(x, y, x_a, y_a, x_c, y_c);
    float denom = f(x_b, y_b, x_a, y_a, x_c, y_c);
    return numerator / denom;
}

/*
 * Calculates gamma, which is used to compute barycentric coordinates.
 */
float compute_gamma(int x_a, int y_a, int x_b, int y_b, int x_c, int y_c,
        int x, int y) {
    float numerator = f(x, y, x_a, y_a, x_b, y_b);
    float denom = f(x_c, y_c, x_a, y_a, x_b, y_b);
    return numerator / denom;
}

/*
 * This is the helper function used to compute alpha, beta, and gamma.
 * Given that this is expressed, as f_{ij}(x, y) in the notes, we have
 * that the parameters are as follows:
 * x: This corresponds to x in the notes.
 * y: This corresponds to y in the notes.
 * x_i: This corresponds to x_i in the notes.
 * y_i: This corresponds to y_i in the notes.
 * x_j: This corresponds to x_j in the notes.
 * y_j: This corresponds to y_j in the notes.
 */
float f(int x, int y, int x_i, int y_i, int x_j, int y_j) {
    float val = (y_i - y_j) * x + (x_j - x_i) * y + x_i * y_j - x_j * y_i;
    return val;
}

/*
 * Creates a ppm struct with the given resolution, based on the passed-in
 * grid.
 */
ppm create_ppm(int xres, int yres, MatrixColor grid) {
    vector<string> lines;
    string header("P3");
    lines.push_back(header);

    ostringstream res_string_stream;
    res_string_stream << xres << " " << yres;
    string res_line = res_string_stream.str();
    lines.push_back(res_line);

    string intensity("255");
    lines.push_back(intensity);

    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            color c = grid(y, x);
            //if (c.r > .1 || c.g > .1 || c.b > .1)
                //cout << "ppming at (y,x) = (" << y << "," << x << ")" << endl;
            lines.push_back(get_line(c));
        }
    }

    ppm p(xres, yres);
    p.lines = lines;
    return p;
}

/*
 * Returns a string representing a ppm line corresponding to the passed-in
 * color.
 */
string get_line(color c) {
    ostringstream color_string_stream;
    color_string_stream << c.r * 255 << " " << c.g * 255 << " " << c.b * 255;
    //color_string_stream << 255 << " " << 0 << " " << 0;
    string color_string = color_string_stream.str();
    return color_string;
}
