#include "parser.h"
#include <Eigen/Dense>

using Eigen::MatrixXd;
using namespace std;

void apply_all_transformations(scene *s);
void apply_all_transformations(object *o, scene *s);
void transform_object_camera(object *o, scene *s);
void transform_object_ndc(object *o, scene *s);
void transform_object_geom(object *o);
MatrixXd get_camera_transform_matrix(vertex position, orientation orient);
MatrixXd get_world_transform_matrix(vertex position, orientation orient);
MatrixXd get_perspective_projection_matrix(scene *s);
