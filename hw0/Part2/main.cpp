#include <cassert>
#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include "matrix_math.h"

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    vector<MatrixXd> matrices;

    // Should only be taking as input a single text file
    assert (argc == 2);
    ifstream infile(argv[1]);
    char determ;
    float x, y, z, angle;
    while (infile >> determ) {
        // rotation lines have 4 nums, all others have 3
        if (determ != 'r') {
            (infile >> x >> y >> z);
            matrices.push_back(get_matrix(determ, x, y, z));
        } else {
            (infile >> x >> y >> z >> angle);
            matrices.push_back(get_matrix(determ, x, y, z, angle));
        }
    }

    MatrixXd product = compute_product(matrices);
    MatrixXd product_inv = product.inverse();
    cout << product_inv << endl;
}
