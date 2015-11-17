#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <Eigen/Dense>

using namespace Eigen;

struct Vec3f
{
	double x, y, z;
    Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3f() {}
};

struct Vertex
{
    float x, y, z;
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
    Vertex() {}
    Vector3f get_vec() {
        Vector3f vec(x, y, z);
        return vec;
    }
};

struct Face
{
    int idx1, idx2, idx3;
    Face(float idx1, float idx2, float idx3) : idx1(idx1), idx2(idx2),
            idx3(idx3) {}
};

struct Mesh_Data
{
    std::vector<Vertex*> *vertices;
    std::vector<Face*> *faces;
};

#endif
