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
    float x, y, z, r, g, b, angle, perspective_param, atten;
    char garbage;

    /*
     * Read file line by line until we get to the line that says "objects:".
     * Then call the method parse_objects to get the vector of objects.
     */
    while (getline(infile, line)) {
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
            } else if (determ.compare("light") == 0) {
                (iss >> x >> y >> z >> garbage >> r >> g >> b >> garbage >> atten);
                light *l = new light(x, y, z, r, g, b, atten);
                s->lights.push_back(l);
            }
            determ = "";
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
    string shading_determ;
    float x, y, z, r, g, b;
    float shine;
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
        /* This reads shading fields for the second part of the file. */
        (iss >> shading_determ);
        if (shading_determ.compare("ambient") == 0) {
            (iss >> r >> g >> b);
            color amb(r, g, b);
            object_copy->ambient = amb;
        } else if (shading_determ.compare("diffuse") == 0) {
            (iss >> r >> g >> b);
            color diff(r, g, b);
            object_copy->diffuse = diff;
        } else if (shading_determ.compare("specular") == 0) {
            (iss >> r >> g >> b);
            color spec(r, g, b);
            object_copy->specular = spec;
        } else if (shading_determ.compare("shininess") == 0) {
            (iss >> shine);
            object_copy->shininess = shine;
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
 * Creats an object from a file, and returns a pointer to it.
 */
object *parse_object(const char *filename) {
    string rel_filename = DATA_DIR;
    rel_filename.append(filename);
    ifstream infile(rel_filename);
    string line;
    string determ;
    float x, y, z;
    string mult_x, mult_y, mult_z;
    vector<vertex *> vertices;
    vector<surface_normal *> normals;
    vector<face *> faces;
    // Vertices are 1-indexed, so push null for 0th element
    vertices.push_back(NULL);
    // Normals are 1-indexed too, so push null for 0th element
    normals.push_back(NULL);

    // Go through file and read vertices and faces, create structs
    while (getline(infile, line)) {
        istringstream iss(line);
        (iss >> determ);
        // Create new vertex
        if (determ.compare("v") == 0) {
            (iss >> x >> y >> z);
            vertex *v = new vertex(x, y, z);
            vertices.push_back(v);
        } else if (determ.compare("vn") == 0) {
            (iss >> x >> y >> z);
            surface_normal *n = new surface_normal(x, y, z);
            normals.push_back(n);
        } else {
            (iss >> mult_x >> mult_y >> mult_z);
            int v1 = mult_x.at(0) - '0';
            int vn1 = mult_x.at(mult_x.size() - 1) - '0';
            int v2 = mult_y.at(0) - '0';
            int vn2 = mult_y.at(mult_y.size() - 1) - '0';
            int v3 = mult_z.at(0) - '0';
            int vn3 = mult_z.at(mult_z.size() - 1) - '0';
            face *f = new face(v1, v2, v3, vn1, vn2, vn3);
            faces.push_back(f);
        }
    }

    object *o = new object(vertices, normals, faces);
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
    // Make input vector a unit vector
    float magnitude = sqrt(x * x + y * y + z * z);
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;

    m << (x * x) + (1 - (x * x)) * cos(angle), (x * y) * (1 - cos(angle)) - z * sin(angle),
              (x * z) * (1 - cos(angle)) + y * sin(angle), 0,
      (y * x) * (1 - cos(angle)) + z * sin(angle), (y * y) + (1 - y * y) * cos(angle),
              (y * z) * (1 - cos(angle)) - x * sin(angle), 0,
      (z * x) * (1 - cos(angle)) - y * sin(angle), (z * y) * (1 - cos(angle)) + x * sin(angle),
              (z * z) + (1 - (z * z)) * cos(angle), 0,
      0, 0, 0, 1;
    return m;
}
