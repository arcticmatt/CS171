#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include "parser.h"
#include <Eigen/Dense>

using Eigen::MatrixXd;
using namespace std;

/*******************************************************************************
 * Defines methods needed for transforming/mapping vertices
 ******************************************************************************/

/* Part 4 */
void map_to_screen_coords(scene *s, int xres, int yres);
void map_to_screen_coords(object *o, int xres, int yres);
/* Parts 2-3 */
void transform_vertex(vertex *v, MatrixXd mat);
void transform_vertices(object *o, MatrixXd mat);
void transform_normals(object *o, MatrixXd mat);
void transform_normal(object *o, MatrixXd mat);
void normalize_normals(object *o);
void transform_object_camera(object *o, scene *s);
void transform_object_ndc(object *o, scene *s);
void transform_object_geom(object *o);
void transform_object_normals(object *o);
MatrixXd get_camera_transform_matrix(vertex position, orientation orient);
MatrixXd get_world_transform_matrix(vertex position, orientation orient);
MatrixXd get_perspective_projection_matrix(scene *s);

#endif
