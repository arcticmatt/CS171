/* Basically just code from first 2 parts */

#include <stdio.h>
#include <vector>
#include <string>
#include <Eigen/Dense>

using Eigen::MatrixXd;
using namespace std;

// Vertex struct contains the three points the vertex is defined by
struct vertex {
    float x;
    float y;
    float z;
    vertex(float x, float y, float z) : x(x), y(y), z(z) {}
    vertex(vertex& other) : x(other.x), y(other.y), z(other.z) {}
};

// Face struct contains the numbers of the vertices the face is composed of
// (1-indexed)
struct face {
    int v1;
    int v2;
    int v3;
    face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
    face(face& other) : v1(other.v1), v2(other.v2), v3(other.v3) {}
};

// Objects contain a list of vertices (1-indexed) and faces
struct object {
    vector<vertex *> vertices;
    vector<face *> faces;
    string label;
    object(vector<vertex *> vertices, vector<face *> faces, string label) : vertices(vertices),
        faces(faces), label(label) {}
    object(object& other) {
        vector<vertex *> new_vertices;
        for (vertex *v : other.vertices) {
            vertex *v2 = NULL;
            if (v != NULL) // 0 index is null
                 v2 = new vertex(*v);
            new_vertices.push_back(v2);
        }
        vertices = new_vertices;

        vector<face *> new_faces;
        for (face *f : other.faces) {
            face *f2 = new face(*f);
            new_faces.push_back(f2);
        }
        faces = new_faces;

        label = other.label;
    }
    ~object() {
        for (vertex *v : vertices) {
            if (v != NULL)
                delete v;
        }
        for (face *f : faces) {
            if (f != NULL)
                delete f;
        }
    }
};

object *read_object(const char *filename);
MatrixXd compute_product(vector<MatrixXd> matrices);
MatrixXd get_matrix(char determ, float x, float y, float z, float angle = 0);
MatrixXd get_translation_matrix(float x, float y, float z);
MatrixXd get_scaling_matrix(float x, float y, float z);
MatrixXd get_rotation_matrix(float x, float y, float z, float angle);
