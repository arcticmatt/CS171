/*** For parsing graphics files. Contains a lot of copied code from Assignment 0 ***/

#include "parser.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <string>

using namespace std;
const string DATA_DIR = "../data/";

/*
 * Given a scene description file of the format specified in HW1 Part 4,
 * reads through it and returns a pointer to a scene struct.
 */
scene *parse_scene(ifstream &infile) {
    scene *s = new scene();
    string line;
    string determ;
    float x, y, z, angle, perspective_param;

    /*
     * Read file line by line until we get to the line that says "objects:".
     * Then call the method parse_objects to get the vector of objects.
     */
    while (getline(infile, line)) {
        cout << line << endl;
        if (line.compare("objects:") == 0) {
            s->objects = parse_objects(infile);
            break;
        } else {
            istringstream iss(line);
            (iss >> determ);
            if (determ.compare("camera") == 0) {
                continue;
            } else if (determ.compare("position") == 0) {
                (iss >> x >> y >> z);
                vertex position(x, y, z);
                s->position = position;
            } else if (determ.compare("orientation") == 0) {
                (iss >> x >> y >> z >> angle);
                orientation orient(x, y, z, angle);
                s->orient = orient;
            } else if (determ.compare("near") == 0) {
                (iss >> perspective_param);
                s->near = perspective_param;
            } else if (determ.compare("far") == 0) {
                (iss >> perspective_param);
                s->far = perspective_param;
            } else if (determ.compare("left") == 0) {
                (iss >> perspective_param);
                s->left = perspective_param;
            } else if (determ.compare("right") == 0) {
                (iss >> perspective_param);
                s->right = perspective_param;
            } else if (determ.compare("top") == 0) {
                (iss >> perspective_param);
                s->top = perspective_param;
            } else if (determ.compare("bottom") == 0) {
                (iss >> perspective_param);
                s->bottom = perspective_param;
            }
        }
    }

    return s;
}

/*
 * Given an input file of the format specified in HW0 Part 3, reads through it and
 * returns a vector of pointers to object structs. Each object struct has a
 * vector of pointers to vertices, a vector of pointers to faces, and a vector
 * of transformation matrices. Each object struct also has a label.
 */
vector<object *> parse_objects(ifstream &infile) {
    unordered_map<string, object *> labels_map;
    vector<object *> object_copies;

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
            object *o = parse_object(filename.c_str());
            o->label = label;
            labels_map[label] = o;
            continue;
        }

        iss.str(line);
        iss.clear();
        /* This reads labels for the second part of the file. */
        if (iss >> label) {
            // Get current object from map we populated when reading first part
            // of file
            curr_object = labels_map[label];
            object_copy = new object(*curr_object);
            object_copies.push_back(object_copy);
        }
    }

    // Free memory
    for (unordered_map<string, object *>::iterator iter = labels_map.begin();
            iter != labels_map.end(); ++iter) {
        object *o = iter->second;
        delete o;
    }

    return object_copies;
}

/*
 * Given a vector of pointers to objects (as returned by the above method),
 * transform the vertices of the objects using the transformation matrices of
 * the objects and output the results.
 *
 * If transform == false, just output the objects as they are.
 */
void output_transformed_objects(vector<object *> objects, bool transform) {
    // We should now have vectors of object copies and matrix products.
    // So now we just need to go through, apply the matrix products to the
    // object vertices, and then print.
    for (int i = 0; i < objects.size(); i++) {
        object *o = objects[i];
        MatrixXd m2 = compute_product(o->transformations);
        MatrixXd m = m2.inverse();
        cout << o->label << " copy" << endl;
        for (vertex *v : o->vertices) {
            if (v == NULL)
                continue;
            MatrixXd vec(4,1);
            vec << v->x, v->y, v->z, 1;
            MatrixXd t_vec = m * vec;
            // Adjust the vector back to 3 dimensions, and change the vertex
            float w = t_vec(3, 0);
            if (transform) {
                v->x = t_vec(0,0) / w;
                v->y = t_vec(1,0) / w;
                v->z = t_vec(2,0) / w;
                cout << v->x << " " << v->y << " " << v->z << endl;
            } else {
                cout << v->x << " " << v->y << " " << v->z << endl;
            }
        }
        cout << "\n";
    }
}

/*
 * Creats an object from a file, and returns a pointer to it.
 */
object *parse_object(const char *filename) {
    string rel_filename = DATA_DIR;
    rel_filename.append(filename);
    ifstream infile(rel_filename);
    char determ;
    float x, y, z;
    vector<vertex *> vertices;
    vector<face *> faces;
    // Vertices are 1-indexed, so push null for 0th element
    vertices.push_back(NULL);

    // Go through file and read vertices and faces, create structs
    while (infile >> determ >> x >> y >> z) {
        // Create new vertex
        if (determ == 'v') {
            vertex *v = new vertex(x, y, z);
            vertices.push_back(v);
        } else {
            face *f = new face((int) x, (int) y, (int) z);
            faces.push_back(f);
        }
    }

    object *o = new object(vertices, faces);
    return o;
}

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
