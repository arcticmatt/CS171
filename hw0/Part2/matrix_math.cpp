#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include "matrix_math.h"

/*
 * Compute the product of a vector of matrixes. Given that the vector contains
 * matrices A, B, and C, in that order, we will compute the product CBA.
 */
MatrixXd compute_product(vector<MatrixXd> matrices) {
    MatrixXd prod = matrices[matrices.size() - 1];
    for (int i = matrices.size() - 2; i >= 0; i--) {
        prod = prod * matrices[i];
    }
    return prod;
}

/*
 * From character determ, determines which matrix (of translation/scaling/rotation)
 * to get, and returns it.
 */
MatrixXd get_matrix(char determ, float x, float y, float z, float angle) {
    if (determ == 't') {
        // Translation matrix
        return get_translation_matrix(x, y, z);
    } else if (determ == 's') {
        // Scaling matrix
        return get_scaling_matrix(x, y, z);
    } else if (determ == 'r') {
        // Rotation matrix
        return get_rotation_matrix(x, y, z, angle);
    } else {
        cout << "Invalid argument, exiting" << endl;
        exit(EXIT_FAILURE);
    }
}

/*
 * Creates the translation matrix corresponding to the (x, y, z) vector and
 * returns it.
 */
MatrixXd get_translation_matrix(float x, float y, float z) {
    MatrixXd m(4,4);
    m << 1, 0, 0, x,
      0, 1, 0, y,
      0, 0, 1, z,
      0, 0, 0, 1;
    return m;
}

/*
 * Creates the scaling matrix corresponding to the (x, y, z) vector and
 * returns it.
 */
MatrixXd get_scaling_matrix(float x, float y, float z) {
    MatrixXd m(4,4);
    m << x, 0, 0, 0,
      0, y, 0, 0,
      0, 0, z, 0,
      0, 0, 0, 1;
    return m;
}

/*
 * Creates the rotation matrix corresponding to the (x, y, z) vector and the
 * passed-in angle and returns it.
 */
MatrixXd get_rotation_matrix(float x, float y, float z, float angle) {
    MatrixXd m(4,4);
    m << (x * x) + (1 - (x * x)) * cos(angle), (x * y) * (1 - cos(angle)) - z * sin(angle),
              (x * z) * (1 - cos(angle)) + y * sin(angle), 0,
      (y * x) * (1 - cos(angle)) + z * sin(angle), (y * y) + (1 - y * y) * cos(angle),
              (y * z) * (1 - cos(angle)) - x * sin(angle), 0,
      (z * x) * (1 - cos(angle)) - y * sin(angle), (z * y) * (1 - cos(angle)) + x * sin(angle),
              (z * z) + (1 - (z * z)) * cos(angle), 0,
      0, 0, 0, 1;
    return m;
}
