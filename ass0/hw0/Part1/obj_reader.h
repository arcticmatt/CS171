#include <stdio.h>
#include <vector>

using namespace std;

// Vertex struct contains the three points the vertex is defined by
struct vertex {
    float x;
    float y;
    float z;
    vertex(float x, float y, float z) : x(x), y(y), z(z) {}
};

// Face struct contains the numbers of the vertices the face is composed of
// (1-indexed)
struct face {
    int v1;
    int v2;
    int v3;
    face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
};

// Objects contain a list of vertices (1-indexed) and faces
struct object {
    vector<vertex *> vertices;
    vector<face *> faces;
    object(vector<vertex *> vertices, vector<face *> faces) : vertices(vertices),
        faces(faces) {}
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
