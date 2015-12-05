#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include "model.hpp"

class Camera;

class Assignment {
    public:
        Assignment() = default;

        static void drawIOTest();
        static bool isInsideRen(float i, float j, float k, Renderable* ren,
                vector<Transformation> transformations, int depth);
        static bool isInsideObj(float i, float j, float k, Object* obj, int depth);
        static bool isInsidePrm(float i, float j, float k, Primitive* prm,
                vector<Transformation> transformations);
        static bool isInside(float i, float j, float k);
        static void drawSphere(float r, float g, float b, float i, float j, float k);
        static float randFloat(float lo, float hi);
        static void drawIntersectTest(Camera *camera);
};

MatrixXd get_inverse_prod(vector<Transformation> transformations);
MatrixXd get_matrix(Transformation transf);
MatrixXd get_translation_matrix(float x, float y, float z);
MatrixXd get_scaling_matrix(float x, float y, float z);
MatrixXd get_rotation_matrix(float x, float y, float z, float angle);

#endif
