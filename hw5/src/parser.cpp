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
Scene *parse_scene(ifstream &infile) {
    Scene *s = new Scene();
    string line;
    string determ;
    float x, y, z, r, g, b, angle, perspective_param, atten;
    char garbage;
    vector<Object *> object_pointers;

    /*
     * Read file line by line until we get to the line that says "objects:".
     * Then call the method parse_objects to get the vector of objects.
     */
    while (getline(infile, line)) {
        if (line.compare("objects:") == 0) {
            object_pointers = parse_objects(infile);
            break;
        } else {
            istringstream iss(line);
            (iss >> determ);
            if (determ.compare("camera") == 0) {
                continue;
            } else if (determ.compare("position") == 0) {
                (iss >> x >> y >> z);
                Vec3f cam_position(x, y, z);
                s->cam_position = cam_position;
            } else if (determ.compare("orientation") == 0) {
                (iss >> x >> y >> z >> angle);
                Vec3f cam_orientation_axis(x, y, z);
                s->cam_orientation_axis = cam_orientation_axis;
                s->cam_orientation_angle = angle;
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
                // Set w component to 1
                Point_Light l(x, y, z, 1, r, g, b, atten);
                s->lights.push_back(l);
            }
            determ = "";
        }
    }

    s->objects = object_list_converter(object_pointers);

    // Free memory
    for (Object *o : object_pointers)
        delete(o);

    return s;
}

/*
 * Convert list of pointers of Objects to list of Objects.
 */
vector<Object> object_list_converter(vector<Object *> pointer_list) {
    vector<Object> object_list;
    for (Object *o : pointer_list) {
        Object obj(*o);
        object_list.push_back(obj);
    }
    return object_list;
}

/*
 * Given an input file of the format specified in HW0 Part 3, reads through it
 * and returns a vector of pointers to object structs. Each object struct has a
 * vector of pointers to vertices, a vector of pointers to faces, and a vector
 * of transformation matrices. Each object struct also has a label.
 */
vector<Object *> parse_objects(ifstream &infile) {
    unordered_map<string, Object *> labels_map;
    vector<Object *> object_copies;

    string line;
    string label, filename;
    char determ;
    string shading_determ;
    float x, y, z, r, g, b;
    float shine;
    float angle = 0;
    Object *curr_object;
    Object *object_copy;

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
            Transforms transform(determ, x, y, z, angle);
            object_copy->transform_sets.push_back(transform);
            continue;
        }

        iss.str(line);
        iss.clear();
        /* This reads shading fields for the second part of the file. */
        (iss >> shading_determ);
        if (shading_determ.compare("ambient") == 0) {
            (iss >> r >> g >> b);
            object_copy->set_ambient(r, g, b);
            continue;
        } else if (shading_determ.compare("diffuse") == 0) {
            (iss >> r >> g >> b);
            object_copy->set_diffuse(r, g, b);
            continue;
        } else if (shading_determ.compare("specular") == 0) {
            (iss >> r >> g >> b);
            object_copy->set_specular(r, g, b);
            continue;
        } else if (shading_determ.compare("shininess") == 0) {
            (iss >> shine);
            object_copy->shininess = shine;
            continue;
        }

        iss.str(line);
        iss.clear();
        /* This reads the first part of the file, labels and filenames. */
        if (iss >> label >> filename) {
            // For each filename, create the corresponding object and put it in
            // the unordered_map
            Object *o = parse_object(filename.c_str());
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
            // TODO: make sure copying works
            object_copy = new Object(*curr_object);
            object_copies.push_back(object_copy);
        }
    }

    return object_copies;
}

/*
 * Creats an object from a file, and returns a pointer to it.
 */
Object *parse_object(const char *filename) {
    string rel_filename = DATA_DIR;
    rel_filename.append(filename);
    ifstream infile(rel_filename);
    string line;
    string determ;
    float x, y, z;
    int idx1, idx2, idx3;
    string mult_x, mult_y, mult_z;
    vector<Vertex> vertex_buffer;
    vector<Vertex> unique_vertices;
    vector<Vertex *> unique_vertex_pointers;
    vector<Face> faces;
    vector<Face *> face_pointers;
    // Vertices are 1-indexed, so push null for 0th element
    unique_vertices.push_back(Vertex());
    unique_vertex_pointers.push_back(NULL);

    // Go through file and read vertices and faces, create structs
    while (getline(infile, line)) {
        istringstream iss(line);
        (iss >> determ);
        // Create new vertex
        if (determ.compare("v") == 0) {
            (iss >> x >> y >> z);
            Vertex *v = new Vertex(x, y, z);
            unique_vertices.push_back(*v);
            unique_vertex_pointers.push_back(v);
        } else if (determ.compare("f") == 0) {
            (iss >> idx1 >> idx2 >> idx3);
            // Push back vertices of the face, and pointers to these vertices
            vertex_buffer.push_back(unique_vertices[idx1]);
            vertex_buffer.push_back(unique_vertices[idx2]);
            vertex_buffer.push_back(unique_vertices[idx3]);

            // Push back the face, and a pointer to the face
            Face *f = new Face(idx1, idx2, idx3);
            faces.push_back(*f);

            face_pointers.push_back(f);
        }
    }

    Object *o = new Object();
    o->unique_vertices = unique_vertices;
    o->vertex_buffer = vertex_buffer;
    o->faces = faces;
    o->unique_vertex_pointers = unique_vertex_pointers;
    o->face_pointers = face_pointers;

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
