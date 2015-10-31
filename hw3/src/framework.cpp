#include "framework.h"
#include <iostream>

/*
 * Go through vector of lights and print the contents.
 */
void output_lights(vector<light *> lights) {
    for (light *l : lights)
        output_light(l);
}

/*
 * Print out a single light.
 */
void output_light(light *l) {
    vertex p = l->position;
    color c = l->colr;
    cout << "light " << p.x << " " << p.y << " " << p.z << " , " <<
        c.r << " " << c.g << " " << c.b << " , " <<
        l->attenuation << endl;
}

/*
 * For every object in a vector of objects, prints out its surface normals.
 */
void output_object_normals(vector<object *> objects) {
    for (object *o : objects)
        output_object_normals(o);
}

/*
 * Prints out all the surface normals of an object.
 */
void output_object_normals(object *o) {
    for (surface_normal *n : o->normals) {
        if (n != NULL) {
            cout << "vn " << n->x << " " << n->y << " " << n->z << endl;
        }
    }
}

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
            cout << "f " << f->v1 << "//" << f->vn1 << " "
                << f->v2 << "//" << f->vn2 << " "
                << f->v3 << "//" << f->vn3 << endl;
        }
    }
}
