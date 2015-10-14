#include <stdio.h>
#include <cmath>
#include "transformer.h"
using namespace std;

/*
 * For every object in a scene, initialize the screen coordinate members of all
 * its vertices (using the overloaded method).
 */
void map_to_screen_coords(scene *s, int xres, int yres) {
    for (object *o : s->objects)
        map_to_screen_coords(o, xres, yres);
}

/*
 * Goees through all vertices of an object and initializes their screen_x and
 * screen_y members. These memebers show how the NDC points map to screen coordinates
 * in a yres-by-xres pixel grid.
 */
void map_to_screen_coords(object *o, int xres, int yres) {
    for (vertex *v : o->vertices) {
        if (v == NULL)
            continue;
        // Check to see if points fall outside of perspective cube
        if (abs(v->x) >= 1 || abs(v->y) >= 1)
            continue;
        // Add one to make sure everything is non-negative, and then divide by
        // 2 to scale everything back (because v->x and v->y now range from 0-2)
        v->screen_x = (v->x + 1) * xres / 2;
        v->screen_y = (v->y + 1) * yres / 2;
    }
}

/*
 * Applies all relevant transformations to every object in the scene.
 */
void apply_all_transformations(scene *s) {
    for (object *o : s->objects)
        apply_all_transformations(o, s);
}

/*
 * Given an object, applies all relevant transformations to it.
 */
void apply_all_transformations(object *o, scene *s) {
    // Apply geometric transforms specified by matrices stored by object
    transform_object_geom(o);
    // Apply world -> camera transform
    transform_object_camera(o, s);
    // Apply camera -> Cartesian NDC transform
    transform_object_ndc(o, s);
}

/*
 * Applies a transformation (specified by a matrix) to the vertices of an
 * object.
 */
void transform_object(object *o, MatrixXd mat) {
    for (vertex *v : o->vertices) {
        if (v == NULL)
            continue;
        MatrixXd vec(4,1);
        vec << v->x, v->y, v->z, 1;
        MatrixXd t_vec = mat * vec;
        // Adjust the vector back to 3 dimensions, and change the vertex
        float w = t_vec(3, 0);
        v->x = t_vec(0,0) / w;
        v->y = t_vec(1,0) / w;
        v->z = t_vec(2,0) / w;
    }
}

/*
 * Applies a geometric transformation on an object given the vector of
 * transformation matrices it has stored.
 */
void transform_object_geom(object *o) {
    MatrixXd m = compute_product(o->transformations);
    transform_object(o, m);
}

/*
 * Applies the world -> camera transformation to an object.
 */
void transform_object_camera(object *o, scene *s) {
    MatrixXd world_transf_mat = get_world_transform_matrix(s->position, s->orient);
    transform_object(o, world_transf_mat);
}

/*
 * Applies the perspective projection matrix to an object, transforming a point
 * in camera space to the Cartesian NDC (because we divide all the terms by w_{ndc})
 */
void transform_object_ndc(object *o, scene *s) {
    MatrixXd ndc_transf_mat = get_perspective_projection_matrix(s);
    transform_object(o, ndc_transf_mat);
}

/*
 * Calculates and returns the matrix that specifies in world space a viewing
 * location and angle from which we look. That is, when we set up a camera,
 * we always rotate it by a specified angle about a specified axis and then
 * translate it to a specified point. In other words, we apply the translation
 * matrix C = T_cR_c.
 */
MatrixXd get_camera_transform_matrix(vertex position, orientation orient) {
    MatrixXd transl_mat = get_translation_matrix(position.x, position.y,
            position.z);
    MatrixXd rot_mat = get_rotation_matrix(orient.x, orient.y,
            orient.z, orient.angle);
    vector<MatrixXd> matrices;
    matrices.push_back(transl_mat);
    matrices.push_back(rot_mat);
    MatrixXd cam_transf_mat = compute_product(matrices);
    return cam_transf_mat;
}

/*
 * Calculates and returns the matrix that transforms world coordinates to
 * camera coordinates. To do this, we just need to multiply by the inverse
 * of the camera transform. So this just returns the inverse.
 */
MatrixXd get_world_transform_matrix(vertex position, orientation orient) {
    MatrixXd cam_transf_mat = get_camera_transform_matrix(position, orient);
    return cam_transf_mat.inverse();
}

/*
 * Given a scene, return the perspective projection matrix for that scene as
 * described in the assignment.
 */
MatrixXd get_perspective_projection_matrix(scene *s) {
    float n = s->near;
    float f = s->far;
    float r = s->right;
    float l = s->left;
    float b = s->bottom;
    float t = s->top;
    MatrixXd m(4, 4);
    m <<
        (2 * n) / (r - l), 0, (r + l) / (r - l), 0,
        0, (2 * n) / (t - b), (t + b) / (t - b), 0,
        0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n),
        0, 0, -1, 0;
    return m;
}
