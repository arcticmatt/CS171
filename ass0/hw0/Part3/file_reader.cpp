#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cassert>
#include <Eigen/Dense>
#include "utils.h"

vector<object *> get_objects(const char* input_file);
void output_transformed_objects(vector<object *> objects);

using Eigen::MatrixXd;
using namespace std;

int main(int argc, const char* argv[]) {
    // Should only be one argument, the input file
    assert (argc == 2);
    // Print the transformed objects!
    vector<object *> objects = get_objects(argv[1]);
    output_transformed_objects(objects);
}

/*
 * Given an input file of the format specified in Part 3, reads through it and returns
 * a vector of pointers to object structs. Each object struct has a vector of
 * pointers to vertices, a vector of pointers to faces, and a vector of transformation
 * matrices. Each object struct also has a label.
 */
vector<object *> get_objects(const char* input_file) {
    unordered_map<string, object *> labels_map;
    vector<object *> object_copies;

    ifstream infile(input_file);
    string line;
    string label, filename;
    char determ;
    float x, y, z;
    float angle = 0;
    object *curr_object;
    object *object_copy;

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
            // Add transformation matrix to current object copy
            object_copy->transformations.push_back(matrix);
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
            // Get current object from map we populated when reading first part of file
            curr_object = labels_map[label];
            object_copy = new object(*curr_object);
            object_copies.push_back(object_copy);
        }
    }

    return object_copies;
}

/*
 * Given a vector of pointers to objects (as returned by the above method), outputs
 * the labels for the objects, followed by their transformed vertices.
 */
void output_transformed_objects(vector<object *> objects) {
    // We should now have vectors of object copies and matrix products.
    // So now we just need to go through, apply the matrix products to the
    // object vertices, and then print.
    for (int i = 0; i < objects.size(); i++) {
        object *o = objects[i];
        MatrixXd m = compute_product(o->transformations);
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
