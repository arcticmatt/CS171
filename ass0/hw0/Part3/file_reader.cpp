#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cassert>
#include <Eigen/Dense>
#include "utils.h"

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    unordered_map<string, object *> labels_map;
    vector<MatrixXd> matrices;
    vector<MatrixXd> matrix_prods;
    vector<object *> object_copies;

    // Should only be one argument, the input file
    assert (argc == 2);

    ifstream infile(argv[1]);
    string line;
    string label, filename;
    char determ;
    float x, y, z;
    float angle = 0;
    object *curr_object;

    /*
     * Read file by trying to read each type of line, and resetting if it doesn't
     * work.
     */
    while (getline(infile, line)) {
        istringstream iss(line);
        /* This reads transformations for the second part of the file. */
        if (iss >> determ >> x >> y >> z) {
            if (determ == 'r')
                (iss >> angle);
            MatrixXd matrix = get_matrix(determ, x, y, z, angle);
            matrices.push_back(matrix);
            continue;
        }

        iss.str(line);
        iss.clear();
        /* This reads the first part of the file, labels and filenames. */
        if (iss >> label >> filename) {
            // For each filename, create the corresponding object and put it in
            // the unordered_map
            object *o = read_object(filename.c_str());
            o->label = label;
            labels_map[label] = o;
            continue;
        }

        iss.str(line);
        iss.clear();
        /* This reads labels for the second part of the file. */
        if (iss >> label) {
            // Check to see if we should do any computation for the previous transformations
            if (matrices.size() > 0) {
                MatrixXd product = compute_product(matrices);
                matrix_prods.push_back(product);
            }
            // Get current object from map we populated when reading first part of file
            curr_object = labels_map[label];
            object *object_copy = new object(*curr_object);
            object_copies.push_back(object_copy);
            // New label, so we have new transformation matrices
            matrices.clear();
        }
    }
    MatrixXd product = compute_product(matrices);
    matrix_prods.push_back(product);

    // We should now have vectors of object copies and matrix products.
    // So now we just need to go through, apply the matrix products to the
    // object vertices, and then print.
    for (int i = 0; i < object_copies.size(); i++) {
        object *o = object_copies[i];
        MatrixXd m = matrix_prods[i];
        cout << o->label << " copy" << endl;
        for (vertex *v : o->vertices) {
            if (v == NULL)
                continue;
            MatrixXd vec(4,1);
            vec << v->x, v->y, v->z, 1;
            //cout << vec(0,0) << " " << vec(1,0) << " " << vec(2,0) << " " <<
                //vec(3,0) << endl;
            MatrixXd t_vec = m * vec;
            cout << t_vec(0,0) << " " << t_vec(1,0) << " " << t_vec(2,0) << " " <<
                t_vec(3,0) << endl;
            cout << "\n";
        }
    }
}
