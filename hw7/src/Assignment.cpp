#include "Assignment.hpp"

#include "Utilities.hpp"

#include "Scene.hpp"
#include "UI.hpp"

#include <cstdlib>
#include <cmath>
#include "Eigen/Dense"

using namespace std;
using namespace Eigen;

const int MAX_RECURSION_DEPTH = 1000;
const float wire_col[] = {0.5, 0.5, 0.5};

void Assignment::drawIOTest() {
    /*
     * For each point p of the form
     *
     * (0.5i, 0.5j, 0.5k) where i,j,k in [-10, 10]
     *
     * check if p is inside one of the superquadrics of the scene (using the
     * inside-outside function). Draw a red sphere of radius 0.1 at that location
     * if it's inside a superquadric and a blue sphere if it's not.
     */

    glEnable(GL_COLOR_MATERIAL);

    const Line* curr_state = CommandLine::getState();
    Renderable* ren = NULL;

    if (curr_state)
        ren = Renderable::get(curr_state->tokens[1]);
    else
        exit(1);

    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            for (int k = -10; k <= 10; k++) {
                float x = i * 0.5;
                float y = j * 0.5;
                float z = k * 0.5;
                if (isInsideRen(x, y, z, ren, vector<Transformation>(), 0))
                    drawSphere(1.0, 0, 0, x, y, z); // red
                else
                    drawSphere(0, 0, 1.0, x, y, z); // blue

            }
        }
    }

    glColor3f(wire_col[0], wire_col[1], wire_col[2]);
}

void Assignment::drawSphere(float r, float g, float b, float i, float j, float k) {
    glPushMatrix();
    glColor3f(r, g, b);
    i += randFloat(0.0, 0.05);
    j += randFloat(0.0, 0.05);
    k += randFloat(0.0, 0.05);
    glTranslatef(i, j, k);
    glutSolidSphere(0.02, 10, 10);
    glPopMatrix();
}

float Assignment::randFloat(float lo, float hi) {
    // Rand float between 0.0 and 1.0
    int rand_int = rand();
    float rand = static_cast<float> (rand_int) / static_cast<float>(RAND_MAX);
    float range = hi - lo;
    return lo + range * rand;
}

/*
 * Sees whether point (i,j,k) is inside any of the current scene's
 * superquadrics.
 *
 * Need to iterate through all of Renderable's superquadrics.
 */
bool Assignment::isInsideRen(float i, float j, float k, Renderable* ren,
        vector<Transformation> transformations, int depth) {
    // Cut off recursion if too deep
    depth += 1;
    if (depth > MAX_RECURSION_DEPTH)
        return false;

    // If Renderable
    if (ren->getType() == PRM)
        return isInsidePrm(i, j, k, dynamic_cast<Primitive*>(ren), transformations);
    else if (ren->getType() == OBJ)
        return isInsideObj(i, j, k, dynamic_cast<Object*>(ren), depth);

    return false;
}

/*
 * Sees whether point (i,j,k) is inside the passed-in object. To do this,
 * need to look at all of object's children.
 */
bool Assignment::isInsideObj(float i, float j, float k, Object* obj, int depth) {
    // Cut off recursion if too deep
    depth += 1;
    if (depth > MAX_RECURSION_DEPTH)
        return false;

    // Iterate thru children
    const unordered_map<Name, Child, NameHasher>& child_map = obj->getChildren();
    bool ans = false;
    for (auto it = child_map.begin(); it != child_map.end(); ++it) {
        Child child = it->second;
        Renderable *ren = Renderable::get(child.name);
        ans = ans || isInsideRen(i, j, k, ren, child.transformations, depth);
        // If point is inside one of the superquadrics, return true
        if (ans)
            return ans;
    }

    return false;
}

/*
 * Sees whether point (i,j,k) is inside the passed-in primitive (superquadric).
 */
bool Assignment::isInsidePrm(float i, float j, float k, Primitive* prm,
        vector<Transformation> transformations) {
    MatrixXd transform = get_inverse_prod(transformations);
    Vector4d vec(i, j, k, 1);
    Vector4d transformed = transform * vec;

    // Plug transformed vec into inside-outside function
    float w = transformed(3);
    float x = transformed(0) / w;
    float y = transformed(1) / w;
    float z = transformed(2) / w;
    float e = prm->getExp0();
    float n = prm->getExp1();
    float inside_outside = pow((pow((x * x), 1.0 / e) + pow(y * y, 1.0 / e)), e / n) +
        pow((z * z), 1.0 / n) - 1.0;
    if (inside_outside < 0)
        return true;

    return false;
}

/*
 * Get inverse product of passed-in transformations.
 */
MatrixXd get_inverse_prod(vector<Transformation> transformations) {
    MatrixXd prod = MatrixXd::Identity(4, 4);
    MatrixXd curr_mat;
    for (int i = 0; i < (int) transformations.size(); i++) {
        curr_mat = get_matrix(transformations[i]);
        curr_mat = curr_mat.inverse();
        prod = prod * curr_mat;
    }

    return prod;
}

/*
 * From character determ, determines which matrix (of translation/scaling/rotation)
 * to get, and returns it.
 */
MatrixXd get_matrix(Transformation transf) {
    TransformationType type = transf.type;
    int x = transf.trans(0);
    int y = transf.trans(1);
    int z = transf.trans(2);
    int angle = transf.trans(3);
    if (type == TRANS) {
        // Translation matrix
        return get_translation_matrix(x, y, z);
    } else if (type == SCALE) {
        // Scaling matrix
        return get_scaling_matrix(x, y, z);
    } else if (type == ROTATE) {
        // Rotation matrix
        return get_rotation_matrix(x, y, z, angle);
    } else {
        cout << "Invalid argument, exiting" << endl;
        exit(EXIT_FAILURE);
    }
}

/*
 * Creates the translation matrix corresponding to the (x, y, z) vector and
 * returns it.
 */
MatrixXd get_translation_matrix(float x, float y, float z) {
    MatrixXd m(4,4);
    m << 1, 0, 0, x,
      0, 1, 0, y,
      0, 0, 1, z,
      0, 0, 0, 1;
    return m;
}

/*
 * Creates the scaling matrix corresponding to the (x, y, z) vector and
 * returns it.
 */
MatrixXd get_scaling_matrix(float x, float y, float z) {
    MatrixXd m(4,4);
    m << x, 0, 0, 0,
      0, y, 0, 0,
      0, 0, z, 0,
      0, 0, 0, 1;
    return m;
}

/*
 * Creates the rotation matrix corresponding to the (x, y, z) vector and the
 * passed-in angle and returns it.
 */
MatrixXd get_rotation_matrix(float x, float y, float z, float angle) {
    MatrixXd m(4,4);
    // Make input vector a unit vector
    float magnitude = sqrt(x * x + y * y + z * z);
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;

    m << (x * x) + (1 - (x * x)) * cos(angle), (x * y) * (1 - cos(angle)) - z * sin(angle),
              (x * z) * (1 - cos(angle)) + y * sin(angle), 0,
      (y * x) * (1 - cos(angle)) + z * sin(angle), (y * y) + (1 - y * y) * cos(angle),
              (y * z) * (1 - cos(angle)) - x * sin(angle), 0,
      (z * x) * (1 - cos(angle)) - y * sin(angle), (z * y) * (1 - cos(angle)) + x * sin(angle),
              (z * z) + (1 - (z * z)) * cos(angle), 0,
      0, 0, 0, 1;
    return m;
}

void Assignment::drawIntersectTest(Camera *camera) {
    cout << "In drawIntersectTest" << endl;
}
