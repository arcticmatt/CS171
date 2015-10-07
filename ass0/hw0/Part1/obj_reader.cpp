#include "obj_reader.h"
#include <iostream>
#include <fstream>
#include <cassert>

/*
 * Creats an object from a file, and returns a pointer to it.
 */
object *read_object(const char *filename) {
    ifstream infile(filename);
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
