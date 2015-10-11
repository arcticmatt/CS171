#include <cassert>
#include <iostream>
#include "obj_reader.h"

using namespace std;

int main(int argc, const char* argv[]) {
    vector<object *> objects;

    // Skip first argument, which is name of program
    for (int i = 1; i < argc; i++) {
        object *o = read_object(argv[i]);
        objects.push_back(o);
    }

    assert(objects.size() == argc - 1);

    // Output the stored object structs we created
    for (int i = 0; i < objects.size(); i++) {
        object *o = objects[i];
        cout << argv[i + 1] << ":" << endl;
        cout << "\n";
        for (vertex *v : o->vertices) {
            if (v != NULL)
                cout << "v " << v->x << " " << v->y << " " << v->z << endl;
        }
        for (face *f : o->faces) {
            cout << "f " << f->v1 << " " << f->v2 << " " << f->v3 << endl;
        }
        cout << "\n";
    }

    // Free memory
    for (object *o : objects)
        delete o;
}
