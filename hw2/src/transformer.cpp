#include <stdio.h>
#include <cmath>
#include <cassert>
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
 * screen_y members. These memebers show how the NDC coordinates map to screen
 * coordinates in a yres-by-xres pixel grid.
 */
void map_to_screen_coords(object *o, int xres, int yres) {
    for (vertex *v : o->vertices) {
        if (v == NULL)
            continue;
        map_to_screen_coords(v, xres, yres);
    }
}

/*
 * Initializes the screen_x and screen_y members of a vertex. These members
 * show how the NDC coordinates map to screen coordinates in a yres-by-xres
 * pixel grid.
 */
void map_to_screen_coords(vertex *v, int xres, int yres) {
    // Add one to make sure everything is non-negative, and then divide by
    // 2 to scale everything back (because v->x and v->y now range from 0-2)
    v->screen_x = (v->x + 1) * xres / 2;
    v->screen_y = (v->y + 1) * yres / 2;
}

/*
 * Get a pointer to a vertex transformed by the passed-in matrix.
 */
vertex *get_transformed_vertex(vertex *v, MatrixXd mat) {
    vertex *copy = new vertex(*v);
    transform_vertex(copy, mat);
    return copy;
}

/*
 * Applies a transformation (specified by a matrix) to a single vertex.
 */
void transform_vertex(vertex *v, MatrixXd mat) {
    assert (v != NULL);

    MatrixXd vec(4,1);
    vec << v->x, v->y, v->z, 1;
    MatrixXd t_vec = mat * vec;

    // Adjust the vector back to 3 dimensions, and change the vertex
    float w = t_vec(3, 0);
    v->x = t_vec(0,0) / w;
    v->y = t_vec(1,0) / w;
    v->z = t_vec(2,0) / w;
}

/*
 * Applies a transformation (specified by a matrix) to the vertices of an
 * object.
 */
void transform_vertices(object *o, MatrixXd mat) {
    for (vertex *v : o->vertices) {
        if (v == NULL)
            continue;
        transform_vertex(v, mat);
    }
}

/*
 * Applies a transformation (specified by a matrix) to a single surface_normal.
 */
void transform_normal(surface_normal *n, MatrixXd mat) {
    assert (n != NULL);

    MatrixXd vec(4,1);
    vec << n->x, n->y, n->z, 1;
    MatrixXd t_vec = mat * vec;
    // Adjust the vector back to 3 dimensions, and change the vertex
    float w = t_vec(3, 0);
    n->x = t_vec(0,0) / w;
    n->y = t_vec(1,0) / w;
    n->z = t_vec(2,0) / w;
}

/*
 * Applies a transformation (specified by a matrix) to the normals of an
 * object.
 */
void transform_normals(object *o, MatrixXd mat) {
    for (surface_normal *n : o->normals) {
        if (n == NULL)
            continue;
        transform_normal(n, mat);
    }
}

/*
 * Normalizes all the normals of an object. Call this after transforming.
 */
void normalize_normals(object *o) {
    for (surface_normal *n : o->normals) {
        if (n == NULL)
            continue;
        float magnitude = sqrt(n->x * n->x + n->y * n->y + n->z * n->z);
        n->x /= magnitude;
        n->y /= magnitude;
        n->z /= magnitude;
    }
}

/*
 * Transform a vertex from world space to camera space and finally to
 * NDC space.
 *
 * IMPORTANT: Before we apply a transformation, go through and save
 * the world coordinates for each vertex. This is so we can interpolate
 * these coordinates for the Phong shading algorithm.
 */
vertex *world_to_ndc(vertex *v, scene *s) {
    if (s->world_to_cam_mat.rows() == 0)
        s->world_to_cam_mat = get_world_transform_matrix(s->position, s->orient);
    if (s->pp_mat.rows() == 0)
        s->pp_mat = get_perspective_projection_matrix(s);

    vertex *transformed = get_transformed_vertex(v, s->world_to_cam_mat);
    transform_vertex(transformed, s->pp_mat);
    // Save world coords
    transformed->world_x = v->x;
    transformed->world_y = v->y;
    transformed->world_z = v->z;
    return transformed;
}

/*
 * Transform each surface normal of an object by the correct normal
 * transformation.
 */
void transform_object_normals(object *o) {
    MatrixXd m = compute_product(o->normal_transformations);
    m = m.inverse().transpose();
    transform_normals(o, m);
}


/*
 * Applies a geometric transformation on an object given the vector of
 * transformation matrices it has stored.
 */
void transform_object_geom(object *o) {
    MatrixXd m = compute_product(o->transformations);
    transform_vertices(o, m);
}

/*
 * Applies the perspective projection matrix to an object, transforming a point
 * in camera space to the Cartesian NDC (because we divide all the terms by w_{ndc})
 *
 */
void transform_object_ndc(object *o, scene *s) {
    MatrixXd ndc_transf_mat;
    if (s->pp_mat.rows() == 0)
        s->pp_mat = get_perspective_projection_matrix(s);
    ndc_transf_mat = s->pp_mat;
    transform_vertices(o, ndc_transf_mat);
}

/*
 * Calculates and returns the matrix that specifies in world space a viewing
 * location and angle from which we look. That is, when we set up a camera,
 * we always rotate it by a specified angle about a specified axis and then
 * translate it to a specified point. In other words, we apply the translation
 * matrix C = T_cR_c.
 *
 * Note that position and orient are the position and orientation of the camera.
 */
MatrixXd get_camera_transform_matrix(vertex position, orientation orient) {
    MatrixXd transl_mat = get_translation_matrix(position.x, position.y,
            position.z);
    MatrixXd rot_mat = get_rotation_matrix(orient.x, orient.y,
            orient.z, orient.angle);
    vector<MatrixXd> matrices;
    matrices.push_back(rot_mat);
    matrices.push_back(transl_mat);
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
