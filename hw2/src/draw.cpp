#include "draw.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

Vector3f lighting(vertex *v, surface_normal *n, surface_material m, scene *s) {
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

        // Scalar prod
        float dot_prod_diff = normal_vec.dot(light_dir);
        Vector3f light_diff = light_col * max(zero, dot_prod_diff);
        diff_sum += light_diff;

        Vector3f h = (cam_dir + light_dir).normalized();
        float dot_prod_spec = normal_vec.dot(h);
        Vector3f light_spec = light_col * pow(max(zero, dot_prod_spec), shine);
        spec_sum += light_spec;
    }

    Vector3f ones(1.0, 1.0, 1.0);
    Vector3f col = col_amb + diff_sum.cwiseProduct(col_diff) +
        spec_sum.cwiseProduct(col_spec);
    Vector3f final_color = ones.cwiseMin(col);
    return final_color;
}
