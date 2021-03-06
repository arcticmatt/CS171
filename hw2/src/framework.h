#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

/*******************************************************************************
 * Defines all the structs
 ******************************************************************************/

// Color struct simply wraps RGB values
struct color {
    float r;
    float g;
    float b;
    color() {}
    color(float r, float g, float b) : r(r), g(g), b(b) {}
    Vector3f get_vec() {
        Vector3f vec(r, g, b);
        return vec;
    }
};

// Defines the material of a surface
struct surface_material {
    color ambient;
    color diffuse;
    color specular;
    float shininess;
};

// Vertex struct contains the three points the vertex is defined by
struct vertex {
    float x;
    float y;
    float z;
    float world_x;
    float world_y;
    float world_z;
    int screen_x = -1;
    int screen_y = -1;
    vertex() {}
    vertex(float x, float y, float z) : x(x), y(y), z(z) {}
    vertex(vertex& other) : x(other.x), y(other.y), z(other.z) {}
    Vector3f get_vec() {
        Vector3f vec(x, y, z);
        return vec;
    }
    Vector3f get_world_vec() {
        Vector3f vec(world_x, world_y, world_z);
        return vec;
    }
};

// Surface Normal struct contains the three points components of a surface normal.
struct surface_normal {
    float x;
    float y;
    float z;
    surface_normal() {}
    surface_normal(float x, float y, float z) : x(x), y(y), z(z) {}
    surface_normal(surface_normal& other) : x(other.x), y(other.y), z(other.z) {}
    Vector3f get_vec() {
        Vector3f vec(x, y, z);
        return vec;
    }
};

// Face struct contains the numbers of the vertices the face is composed of
// (1-indexed), as well as the numbers of the surfaces normals for those
// vertices. So, for example vertex #v1 has the vn1-th surface normal.
// In other words, these numbers index into the vectors that are stored
// in the object struct.
struct face {
    int v1;
    int v2;
    int v3;
    int vn1;
    int vn2;
    int vn3;
    color c1;
    color c2;
    color c3;
    face() {}
    face(int v1, int v2, int v3, int vn1, int vn2, int vn3) :
            v1(v1), v2(v2), v3(v3), vn1(vn1), vn2(vn2), vn3(vn3) {}
    face(face& other) : v1(other.v1), v2(other.v2), v3(other.v3),
            vn1(other.vn1), vn2(other.vn2), vn3(other.vn3) {}
};

// An orientation is just a position as well as an angle
struct orientation {
    float x;
    float y;
    float z;
    float angle;
    orientation() {}
    orientation(float x, float y, float z, float angle) : x(x), y(y), z(z),
            angle(angle) {}
    orientation(orientation& other) : x(other.x), y(other.y), z(other.z),
            angle(other.angle) {}
};

// Lights contain a position (specified by a vertex), a color, and an attenuation
// parameter
struct light {
    vertex position;
    color colr;
    float attenuation;
    light() {}
    light(float x, float y, float z, float r, float g, float b, float atten) {
        position = vertex(x, y, z);
        colr = color(r, g, b);
        attenuation = atten;
    }
    light(light& other) : position(other.position), colr(other.colr),
            attenuation(other.attenuation) {}
};

// Objects contain a list of vertices (1-indexed),
// list of surface normals (also 1-indexed), and faces
struct object {
    vector<vertex *> vertices;
    vector<surface_normal *> normals;
    vector<face *> faces;
    vector<MatrixXd> transformations;
    vector<MatrixXd> normal_transformations;
    surface_material material;
    string label;
    object() {}
    object(vector<vertex *> vertices, vector<surface_normal *> normals,
            vector<face *> faces) : vertices(vertices), normals(normals),
            faces(faces) {}
    object(object& other) {
        vector<vertex *> new_vertices;
        for (vertex *v : other.vertices) {
            vertex *v2 = NULL;
            if (v != NULL) // 0 index is null
                 v2 = new vertex(*v);
            new_vertices.push_back(v2);
        }
        vertices = new_vertices;

        vector<surface_normal *> new_normals;
        for (surface_normal *n : other.normals) {
            surface_normal *n2 = NULL;
            if (n != NULL) // 0 index is null
                 n2 = new surface_normal(*n);
            new_normals.push_back(n2);
        }
        normals = new_normals;

        vector<face *> new_faces;
        for (face *f : other.faces) {
            face *f2 = new face(*f);
            new_faces.push_back(f2);
        }
        faces = new_faces;

        transformations = other.transformations;
        material = other.material;
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
        for (surface_normal *n : normals) {
            if (n != NULL)
                delete n;
        }
    }
};

void output_lights(vector<light *> lights);
void output_light(light *l);
void output_object_normals(vector<object *> objects);
void output_object_normals(object *o);
void output_object_vertices(vector<object *> objects);
void output_object_vertices(object *o);
void output_object_faces(vector<object *> objects);
void output_object_faces(object *o);

/* Contains camera position, list of objects, list of lights */
struct scene {
    vertex position;
    orientation orient;
    float near;
    float far;
    float left;
    float right;
    float top;
    float bottom;
    vector<object *> objects;
    vector<light *> lights;
    MatrixXd pp_mat;
    MatrixXd world_to_cam_mat;
    MatrixXd depth_buffer;
    scene() {}
    ~scene() {
        for (object *o : objects) {
            if (o != NULL)
                delete o;
        }
        for (light *l : lights) {
            if (l != NULL)
                delete l;
        }
    }
    void print() {
        cout << "position " << position.x << " " << position.y << " " << position.z
            << endl;
        cout << "orientation " << orient.x << " " << orient.y << " " << orient.z
            << " " << orient.angle << endl;
        cout << "near " << near << endl;
        cout << "far " << far << endl;
        cout << "left " << left << endl;
        cout << "right " << right << endl;
        cout << "top " << top << endl;
        cout << "bottom " << bottom << endl;
        output_lights(lights);
        output_object_vertices(objects);
        output_object_normals(objects);
        output_object_faces(objects);
    }
};

typedef Matrix<color, Dynamic, Dynamic> MatrixColor;

#endif
