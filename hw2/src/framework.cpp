#include "framework.h"
#include <iostream>

/*
 * For every object in a vector of objects, prints out its vertices.
 */
void output_object_vertices(vector<object *> objects) {
    for (object *o : objects)
        output_object_vertices(o);
}

/*
 * Prints out all the vertices of an object.
 */
void output_object_vertices(object *o) {
    for (vertex *v : o->vertices) {
        if (v != NULL) {
            cout << "v " << v->x << " " << v->y << " " << v->z <<
                " " << v->screen_x << " " << v->screen_y << endl;
        }
    }
}

/*
 * For every object in a vector of objects, prints out its faces.
 */
void output_object_faces(vector<object *> objects) {
    for (object *o : objects)
        output_object_faces(o);
}

/*
 * Prints out all the faces of an object.
 */
void output_object_faces(object *o) {
    for (face *f : o->faces) {
        if (f != NULL) {
            cout << "f " << f->v1 << " " << f->v2 << " " << f->v3 << endl;
        }
    }
}
