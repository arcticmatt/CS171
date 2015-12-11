#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include <vector>

#include "PNGMaker.hpp"
#include "model.hpp"

class Camera;
class Scene;

using namespace std;

class Assignment {
    public:
        Assignment() = default;

        static void raytrace(Camera camera, Scene scene);

        // From part A
        static void traverseRen(Renderable* ren, vector<Transformation> transformations,
                int depth);
        static void traverseObj(Object* obj, int depth);
        static void traversePrm(Primitive* prm, vector<Transformation> transformations);

        static Vector3f* getIntersectNormal(Camera* camera, Scene scene);
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
MatrixXf get_matrix_prod(vector<Transformation> transformations,
        bool exclude_translation = false);
MatrixXf get_inverse_prod(vector<Transformation> transformations,
        bool exclude_translation = false);
MatrixXf get_matrix(Transformation transf);
MatrixXf get_translation_matrix(float x, float y, float z);
MatrixXf get_scaling_matrix(float x, float y, float z);
MatrixXf get_rotation_matrix(float x, float y, float z, float angle);
float deg2rad(float angle);

#endif
