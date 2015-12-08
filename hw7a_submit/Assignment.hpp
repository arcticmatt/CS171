#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include "model.hpp"

class Camera;

class Assignment {
    public:
        Assignment() = default;

        static void drawIOTest();
        static void traverseRen(Renderable* ren, vector<Transformation> transformations,
                int depth);
        static void traverseObj(Object* obj, int depth);
        static void traversePrm(Primitive* prm, vector<Transformation> transformations);
        static bool isInsidePrm(float i, float j, float k);
        static bool isInsidePrm(float i, float j, float k, Primitive* prm,
                vector<Transformation> transformations);
        static bool isInside(float i, float j, float k);
        static void drawSphere(float r, float g, float b, float i, float j, float k);
        static float randFloat(float lo, float hi);

        static void drawIntersectTest(Camera* camera);
        static Vector3f getNormal(Vector3f u_point, Primitive* prm,
                vector<Transformation> transformations);
        static Vector3f transformIntersection(Vector3f u_point,
                vector<Transformation> transformations);
        static Vector3f* intersectPrm(Camera* camera, Primitive* prm,
                vector<Transformation> transformations);
        static vector<float> getInitialGuesses(Vector3f cam_pos_transformed,
                Vector3f cam_dir_transformed);
        static int closestPointIndex(vector<Vector3f> points,
                vector<pair<Primitive*, vector<Transformation>>> pairs, Camera* camera);
};

Vector3f transform_vector(Vector3f v, MatrixXf transform);
MatrixXf get_matrix_prod(vector<Transformation> transformations);
MatrixXf get_inverse_prod(vector<Transformation> transformations,
        bool exclude_translation = false);
MatrixXf get_matrix(Transformation transf);
MatrixXf get_translation_matrix(float x, float y, float z);
MatrixXf get_scaling_matrix(float x, float y, float z);
MatrixXf get_rotation_matrix(float x, float y, float z, float angle);

#endif
