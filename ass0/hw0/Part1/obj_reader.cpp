#include "obj_reader.h"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

int main(int argc, const char* argv[]) {
    vector<object> objects;

    // Skip first argument, which is name of program
    for (int i = 1; i < argc; i++) {
        ifstream infile(argv[i]);
        char determ;
        float x, y, z;
        vector<vertex> vertices;
        vector<face> faces;

        // Go through file and read vertices and faces, create structs
        while (infile >> determ >> x >> y >> z) {
            // Create new vertex
            if (determ == 'v') {
                vertex v(x, y, z);
                vertices.push_back(v);
            } else {
                face f((int) x, (int) y, (int) z);
                faces.push_back(f);
            }
        }

        object o(vertices, faces);
        objects.push_back(o);
    }

    assert(objects.size() == argc - 1);

    // Output the stored object structs we created
    for (int i = 0; i < objects.size(); i++) {
        object o = objects[i];
        cout << argv[i + 1] << ":" << endl;
        for (vertex v : o.vertices) {
            cout << "v " << v.x << " " << v.y << " " << v.z << endl;
        }
        for (face f : o.faces) {
            cout << "f " << f.v1 << " " << f.v2 << " " << f.v3 << endl;
        }
    }
}
