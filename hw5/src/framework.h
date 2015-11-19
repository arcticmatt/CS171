#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "structs.h"
#include "halfedge.h"

using namespace std;

/*******************************************************************************
 * Defines all the structs
 ******************************************************************************/

/* The following struct is used for representing a point light.
 *
 * Note that the position is represented in homogeneous coordinates rather than
 * the simple Cartesian coordinates that we would normally use. This is because
 * OpenGL requires us to specify a w-coordinate when we specify the positions
 * of our point lights. We specify the positions in the 'set_lights' function.
 */
struct Point_Light
{
    /* Index 0 has the x-coordinate
     * Index 1 has the y-coordinate
     * Index 2 has the z-coordinate
     * Index 3 has the w-coordinate
     */
    float position[4];

    /* Index 0 has the r-component
     * Index 1 has the g-component
     * Index 2 has the b-component
     */
    float color[3];

    /* This is our 'k' factor for attenuation as discussed in the lecture notes
     * and extra credit of Assignment 2.
     */
    float attenuation_k;
    Point_Light(float x, float y, float z, float w,
            float r, float g, float b, float atten) {
        position[0] = x;
        position[1] = y;
        position[2] = z;
        position[3] = w;
        color[0] = r;
        color[1] = g;
        color[2] = b;
        attenuation_k = atten;
    }
    Point_Light() {}
};

/* The following struct is used for storing a set of transformations.
 *
 * Note that we do not need to use matrices this time to represent the
 * transformations. This is because OpenGL will handle all the matrix
 * operations for us when we have it apply the transformations. All we
 * need to do is supply the parameters.
 */
struct Transforms
{
    /* For each array below,
     * Index 0 has the x-component
     * Index 1 has the y-component
     * Index 2 has the z-component
     */
    float translation[3] = {0, 0, 0};
    float rotation[3];
    float scaling[3] = {1, 1, 1};

    /* Angle in degrees.
     */
    float rotation_angle = 0;
    char determ;

    Transforms(char d, float x, float y, float z, float angle) {
        determ = d;
        if (d == 'r') {
            rotation[0] = x;
            rotation[1] = y;
            rotation[2] = z;
            rotation_angle = angle;
        } else if (d == 't') {
            translation[0] = x;
            translation[1] = y;
            translation[2] = z;
        } else if (d == 's') {
            scaling[0] = x;
            scaling[1] = y;
            scaling[2] = z;
        }
    }
};

/* The following struct is used to represent objects.
 *
 * The main things to note here are the 'vertex_buffer' and 'normal_buffer'
 * vectors.
 *
 * You will see later in the 'draw_objects' function that OpenGL requires
 * us to supply it all the faces that make up an object in one giant
 * "vertex array" before it can render the object. The faces are each specified
 * by the set of vertices that make up the face, and the giant "vertex array"
 * stores all these sets of vertices consecutively. Our "vertex_buffer" vector
 * below will be our "vertex array" for the object.
 *
 * As an example, let's say that we have a cube object. A cube has 6 faces,
 * each with 4 vertices. Each face is going to be represented by the 4 vertices
 * that make it up. We are going to put each of these 4-vertex-sets one by one
 * into 1 large array. This gives us an array of 36 vertices. e.g.:
 *
 * [face1vertex1, face1vertex2, face1vertex3, face1vertex4,
 *  face2vertex1, face2vertex2, face2vertex3, face2vertex4,
 *  face3vertex1, face3vertex2, face3vertex3, face3vertex4,
 *  face4vertex1, face4vertex2, face4vertex3, face4vertex4,
 *  face5vertex1, face5vertex2, face5vertex3, face5vertex4,
 *  face6vertex1, face6vertex2, face6vertex3, face6vertex4]
 *
 * This array of 36 vertices becomes our 'vertex_array'.
 *
 * While it may be obvious to us that some of the vertices in the array are
 * repeats, OpenGL has no way of knowing this. The redundancy is necessary
 * since OpenGL needs the vertices of every face to be explicitly given.
 *
 * The 'normal_buffer' stores all the normals corresponding to the vertices
 * in the 'vertex_buffer'. With the cube example, since the "vertex array"
 * has "36" vertices, the "normal array" also has "36" normals.
 */
struct Object
{
    /* See the note above and the comments in the 'draw_objects' and
     * 'create_cubes' functions for details about these buffer vectors.
     */
    vector<Vertex> vertex_buffer;
    vector<Vec3f> normal_buffer;

    vector<Face> faces;
    vector<Face *> face_pointers;

    vector<Vertex> unique_vertices;
    vector<Vertex *> unique_vertex_pointers;
    vector<Vec3f> unique_normals;

    vector<Transforms> transform_sets;

    /* Index 0 has the r-component
     * Index 1 has the g-component
     * Index 2 has the b-component
     */
    float ambient_reflect[3] = {5, 5, 5};
    float diffuse_reflect[3];
    float specular_reflect[3];

    float shininess;

    string label;

    Object() {}

    void set_ambient(float r, float g, float b) {
        ambient_reflect[0] = r;
        ambient_reflect[1] = g;
        ambient_reflect[2] = b;
    }

    void set_diffuse(float r, float g, float b) {
        diffuse_reflect[0] = r;
        diffuse_reflect[1] = g;
        diffuse_reflect[2] = b;
    }

    void set_specular(float r, float g, float b) {
        specular_reflect[0] = r;
        specular_reflect[1] = g;
        specular_reflect[2] = b;
    }
};

/* Contains camera position, list of objects, list of lights */
struct Scene {
    Vec3f cam_position;
    Vec3f cam_orientation_axis;
    float cam_orientation_angle;
    float near;
    float far;
    float left;
    float right;
    float top;
    float bottom;
    vector<Point_Light> lights;
    vector<Object> objects;
    vector<Mesh_Data *> meshes;
    vector<vector<HEV *>*> object_hevs;
    vector<vector<HEF *>*> object_hefs;
    int xres;
    int yres;
    Scene() {}

    /*
     * Index HEVs.
     */
    void index_hevs() {
        for (int i = 0; i < object_hevs.size(); i++) {
            // object_hevs[i] = vector<HEV *> *
            for (int j = 1; j < object_hevs[i]->size(); j++) {
                object_hevs[i]->at(j)->index = j; // assign each vertex an index
            }
        }
    }

    /*
     * Get mesh data objects for each object.
     */
    void populate_meshes() {
        for (Object o : objects) {
            Mesh_Data *mesh_data = new Mesh_Data;
            mesh_data->vertices = &o.unique_vertex_pointers;
            mesh_data->faces = &o.face_pointers;

            vector<HEV*> *hevs = new vector<HEV*>();
            vector<HEF*> *hefs = new vector<HEF*>();

            cout << "Before building HE" << endl;
            build_HE(mesh_data, hevs, hefs);
            cout << "After building HE" << endl;
            meshes.push_back(mesh_data);
            object_hevs.push_back(hevs);
            object_hefs.push_back(hefs);
        }
    }

    /*
     * Populate object normals, using half-edges.
     */
    void populate_object_normals() {
        for (int i = 0; i < (int) objects.size(); i++) {
            vector<HEV*> *hevs = object_hevs[i];
            cout << "hevs size = " << hevs->size() << endl;;
            // Unique normals should be 1-indexed
            objects[i].unique_normals.push_back(Vec3f());
            for (HEV *vertex : *hevs) {
                if (vertex == NULL)
                    continue;
                Vec3f normal = calc_vertex_normal(vertex);
                //cout << "n " << normal.x << " " << normal.y << " " << normal.z << endl;
                objects[i].unique_normals.push_back(normal);
            }
        }

        for (int i = 0; i < (int) objects.size(); i++) {
            for (Face f : objects[i].faces) {
                // Use same indices as vertices.
                //cout << "indices = " << f.idx1 << " " << f.idx2 << " " << f.idx3 << endl;
                Vec3f n1 = objects[i].unique_normals[f.idx1];
                //cout << "n1 = " << n1.x << " " << n1.y << " " << n1.z << endl;
                Vec3f n2 = objects[i].unique_normals[f.idx2];
                Vec3f n3 = objects[i].unique_normals[f.idx3];
                objects[i].normal_buffer.push_back(n1);
                objects[i].normal_buffer.push_back(n2);
                objects[i].normal_buffer.push_back(n3);
            }
        }
    }

    /*
     * Given an HEV, calculate it's normal.
     */
    Vec3f calc_vertex_normal(HEV *vertex) {
        Vec3f normal;
        normal.x = 0;
        normal.y = 0;
        normal.z = 0;

        HE* he = vertex->out; // get outgoing halfedge from given vertex

        do
        {
            HEF *hef = he->face;

            HEV *hev1 = hef->edge->vertex;
            HEV *hev2 = hef->edge->next->vertex;
            HEV *hev3 = hef->edge->next->next->vertex;

            Vector3f v1 = hevToVector(hev1);
            Vector3f v2 = hevToVector(hev2);
            Vector3f v3 = hevToVector(hev3);

            // compute the normal of the plane of the face.
            // normal = cross prod of (v2 - v1) x (v3 - v1)
            Vector3f v2_v1 = v2 - v1;
            Vector3f v3_v1 = v3 - v1;
            Vector3f face_normal = v2_v1.cross(v3_v1);
            // compute the area of the triangular face
            // area = 1/2 * |face_normal| TODO check this
            double face_area = 0.5 * face_normal.norm();

            // accummulate onto our normal vector
            normal.x += face_normal(0) * face_area;
            normal.y += face_normal(1) * face_area;
            normal.z += face_normal(2) * face_area;

            // gives us the halfedge to the next adjacent vertex
            he = he->flip->next;
        }
        while(he != vertex->out);

        // NORMALIZE the normal
        float magnitude = sqrt(normal.x * normal.x + normal.y * normal.y +
                normal.z * normal.z);
        normal.x /= magnitude;
        normal.y /= magnitude;
        normal.z /= magnitude;
        vertex->normal = normal;
        return normal;
    }

    /*
     * Converts an HEV to an Eigen vector.
     */
    Vector3f hevToVector(HEV *hev) {
        Vector3f vec(hev->x, hev->y, hev->z);
        return vec;
    }

    /*
     * Convert all angles associated with scene to degrees.
     */
    void convert_to_degrees() {
        cam_orientation_angle *= (180 / M_PI);
        for (int i = 0; i < (int) objects.size(); i++) {
            Object o = objects[i];
            for (int j = 0; j < (int) o.transform_sets.size(); j++) {
                Transforms t = o.transform_sets[j];
                t.rotation_angle *= (180 / M_PI);
                o.transform_sets[j] = t;
            }
            objects[i] = o;
        }
    }

    void print() {
        cout << "position " << cam_position.x << " " << cam_position.y
            << " " << cam_position.z << endl;
        cout << "orientation " << cam_orientation_axis.x << " "
            << cam_orientation_axis.y << " " << cam_orientation_axis.z
            << " " << cam_orientation_angle << endl;
        cout << "near " << near << endl;
        cout << "far " << far << endl;
        cout << "left " << left << endl;
        cout << "right " << right << endl;
        cout << "top " << top << endl;
        cout << "bottom " << bottom << endl;

        cout << "\n";
        for (Point_Light l : lights) {
            cout << "light " << l.position[0] << " " << l.position[1] << " "
                << l.position[2] << " , " << l.color[0] << " " << l.color[1]
                << " " << l.color[2] << " , " << l.attenuation_k << endl;
        }

        cout << "\n";
        for (Object o : objects) {
            cout << o.label << endl;
            cout << "ambient " << o.ambient_reflect[0] << " "
                << o.ambient_reflect[1] << " " << o.ambient_reflect[2] << endl;
            cout << "diffuse " << o.diffuse_reflect[0] << " "
                << o.diffuse_reflect[1] << " " << o.diffuse_reflect[2] << endl;
            cout << "specular " << o.specular_reflect[0] << " "
                << o.specular_reflect[1] << " " << o.specular_reflect[2] << endl;
            cout << "shininess " << o.shininess << endl;
            for (Transforms t : o.transform_sets) {
                if (t.determ == 'r') {
                    cout << "r " << t.rotation[0] << " " << t.rotation[1]
                        << " " << t.rotation[2] << " " << t.rotation_angle << endl;
                } else if (t.determ == 't') {
                    cout << "t " << t.translation[0] << " " << t.translation[1]
                        << " " << t.translation[2] << endl;
                } else if (t.determ == 's') {
                    cout << "s " << t.scaling[0] << " " << t.scaling[1]
                        << " " << t.scaling[2] << endl;
                }
            }
        }

        cout << "faces size = " << objects[0].faces.size() << endl;
        cout << "unique_vertices size = " << objects[0].unique_vertices.size() << endl;
        cout << "unique_normals size = " << objects[0].unique_normals.size() << endl;
        cout << "vertex_buffer size = " << objects[0].vertex_buffer.size() << endl;
        cout << "normal_buffer size = " << objects[0].normal_buffer.size() << endl;
    }
};

#endif
