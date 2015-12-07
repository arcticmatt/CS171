#include "Assignment.hpp"

#include "Utilities.hpp"

#include "Scene.hpp"
#include "UI.hpp"

#include <algorithm>
#include <utility>
#include <cstdlib>
#include <cmath>
#include "Eigen/Dense"

using namespace std;
using namespace Eigen;

vector<pair<Primitive*, vector<Transformation>>> scene_copy;
const int MAX_RECURSION_DEPTH = 1000;
const float wire_col[] = {0.5, 0.5, 0.5};
const float SMALL = 0.001;

/*
 * Inside outside method.
 */
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

    cout << "In drawIOTest" << endl;

    glEnable(GL_COLOR_MATERIAL);

    const Line* curr_state = CommandLine::getState();
    Renderable* ren = NULL;

    if (curr_state)
        ren = Renderable::get(curr_state->tokens[1]);
    else
        return;

    // Populate scene copy with list of primitives and transformations
    scene_copy.clear();
    traverseRen(ren, vector<Transformation>(), 0);

    for (int i = -10; i <= 10; i++) {
        for (int j = -10; j <= 10; j++) {
            for (int k = -10; k <= 10; k++) {
                float x = i * 0.5;
                float y = j * 0.5;
                float z = k * 0.5;
                // Check if point is inside one of the scene's primitives
                if (isInsidePrm(x, y, z))
                    drawSphere(1.0, 0, 0, x, y, z); // red
                else
                    drawSphere(0, 0, 1.0, x, y, z); // blue
            }
        }
    }

    glColor3f(wire_col[0], wire_col[1], wire_col[2]);
}

/*
 * Draw a colored sphere at the passed-in point.
 */
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

/*
 * Generate random float between the passed-in range.
 */
float Assignment::randFloat(float lo, float hi) {
    // Rand float between 0.0 and 1.0
    int rand_int = rand();
    float rand = static_cast<float> (rand_int) / static_cast<float>(RAND_MAX);
    float range = hi - lo;
    return lo + range * rand;
}

/*
 * Traverse renderable & populate scene copy vector.
 */
void Assignment::traverseRen(Renderable* ren, vector<Transformation> transformations,
        int depth) {
    // Cut off recursion if too deep
    depth += 1;
    if (depth > MAX_RECURSION_DEPTH)
        return;

    // If Renderable
    if (ren->getType() == PRM)
        traversePrm(dynamic_cast<Primitive*>(ren), transformations);
    else if (ren->getType() == OBJ)
        traverseObj(dynamic_cast<Object*>(ren), depth);
}

/*
 * Traverse object & populate scene copy vector.
 */
void Assignment::traverseObj(Object* obj, int depth) {
    // Cut off recursion if too deep
    depth += 1;
    if (depth > MAX_RECURSION_DEPTH)
        return;

    // Iterate thru children
    const unordered_map<Name, Child, NameHasher>& child_map = obj->getChildren();
    for (auto it = child_map.begin(); it != child_map.end(); ++it) {
        Child child = it->second;
        Renderable *ren = Renderable::get(child.name);
        traverseRen(ren, child.transformations, depth);
    }
}

/*
 * Traverse primitive & populate scene copy vector.
 */
void Assignment::traversePrm(Primitive* prm, vector<Transformation> transformations) {
    scene_copy.push_back(make_pair(prm, transformations));
}


/*
 * Loops through the vector of scene primitives and checks to see if the passed-in
 * point is inside of any of them.
 */
bool Assignment::isInsidePrm(float i, float j, float k) {
    for (pair<Primitive*, vector<Transformation>> p : scene_copy) {
        if (isInsidePrm(i, j, k, p.first, p.second))
            return true;
    }
    return false;
}

/*
 * Sees whether point (i,j,k) is inside the passed-in primitive (superquadric).
 */
bool Assignment::isInsidePrm(float i, float j, float k, Primitive* prm,
        vector<Transformation> transformations) {
    MatrixXf transform = get_inverse_prod(transformations);
    Vector4f vec(i, j, k, 1);
    Vector4f transformed = transform * vec;

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
 * Transforms a vector.
 */
Vector3f transform_vector(Vector3f v, MatrixXf transform) {
    Vector4f v1(v(0), v(1), v(2), 1);
    Vector4f v_t = transform * v1;
    Vector3f v_f;
    v_f(0) = v_t(0) / v_t(3);
    v_f(1) = v_t(1) / v_t(3);
    v_f(2) = v_t(2) / v_t(3);
    return v_f;
}

/*
 * Get product of passed-in transformations. Compute product in reverse order;
 * that is, if vector is ABC, compute product CBA.
 */
MatrixXf get_matrix_prod(vector<Transformation> transformations) {
    MatrixXf prod = MatrixXf::Identity(4, 4);
    MatrixXf curr_mat;
    for (int i = (int) transformations.size() - 1; i >= 0; i--) {
        curr_mat = get_matrix(transformations[i]);
        prod = prod * curr_mat;
    }

    return prod;
}

/*
 * Get inverse product of passed-in transformations.
 */
MatrixXf get_inverse_prod(vector<Transformation> transformations,
        bool exclude_translation) {
    MatrixXf prod = MatrixXf::Identity(4, 4);
    MatrixXf curr_mat;
    for (int i = 0; i < (int) transformations.size(); i++) {
        if (exclude_translation && transformations[i].type == TRANS)
            continue;
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
MatrixXf get_matrix(Transformation transf) {
    TransformationType type = transf.type;
    float x = transf.trans(0);
    float y = transf.trans(1);
    float z = transf.trans(2);
    float angle = transf.trans(3);
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
MatrixXf get_translation_matrix(float x, float y, float z) {
    MatrixXf m(4,4);
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
MatrixXf get_scaling_matrix(float x, float y, float z) {
    MatrixXf m(4,4);
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
MatrixXf get_rotation_matrix(float x, float y, float z, float angle) {
    MatrixXf m(4,4);
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

/*
 * Raytracing method.
 */
void Assignment::drawIntersectTest(Camera* camera) {
    cout << "In drawIntersectTest" << endl;

    const Line* curr_state = CommandLine::getState();
    Renderable* ren = NULL;

    if (curr_state)
        ren = Renderable::get(curr_state->tokens[1]);
    else
        return;

    // Populate scene copy with list of primitives and transformations
    scene_copy.clear();
    traverseRen(ren, vector<Transformation>(), 0);

    vector<Vector3f> points;
    vector<pair<Primitive*, vector<Transformation>>> pairs;
    // Get intersections with superquadrics
    for (pair<Primitive*, vector<Transformation>> p : scene_copy) {
        Vector3f* intersection = intersectPrm(camera, p.first, p.second);
        // If our intersection is not null, draw the ray
        if (intersection != NULL) {
            points.push_back(*intersection);
            pairs.push_back(p);
        }
    }

    if (points.size() > 0) {
        int min_index = closestPointIndex(points, pairs, camera);
        Vector3f intersection = points[min_index];
        pair<Primitive*, vector<Transformation>> p = pairs[min_index];
        Vector3f norm = getNormal(intersection, p.first, p.second);
        norm.normalize();
        Vector3f t_intersection = transformIntersection(intersection, p.second);
        cout << "Intersection point = \n" << t_intersection << endl;
        cout << "Normal = \n" << norm << endl;

        // Draw the line
        glBegin(GL_LINES);
            Vector3f p1 = t_intersection;
            glVertex3f(p1(0), p1(1), p1(2));
            Vector3f p2 = t_intersection + norm;
            glVertex3f(p2(0), p2(1), p2(2));
        glEnd();
    }
}

/*
 * Gets the normal for the passed-in primitive at the passed-in surface point.
 */
Vector3f Assignment::getNormal(Vector3f u_point, Primitive* prm,
        vector<Transformation> transformations) {
    // Scale the untransformed point using the primitives scaling transformations
    for (Transformation transf : transformations) {
        if (transf.type == SCALE) {
            Vector3f scale(transf.trans(0), transf.trans(1), transf.trans(2));
            u_point = u_point.cwiseProduct(scale);
        }
    }

    Vector3f norm = prm->getNormal(u_point);
    return norm;
}

/*
 * Transforms the intersection point.
 */
Vector3f Assignment::transformIntersection(Vector3f u_point,
        vector<Transformation> transformations) {
    MatrixXf transform = get_matrix_prod(transformations);
    return transform_vector(u_point, transform);
}

/*
 * Checks whether the ray of the passed-in camera intersects the passed-in primitive.
 * Returns the point of intersection.
 */
Vector3f* Assignment::intersectPrm(Camera* camera, Primitive* prm,
        vector<Transformation> transformations) {
    MatrixXf transform = get_inverse_prod(transformations);

    /* Apply inverse transforms to cam position and direction */
    Vector3f cam_pos_transformed = transform_vector(camera->getPosition(),
            transform);

    Vector3f cam_dir(0.0, 0.0, -1.0);
    Vector3f rotation_axis = camera->getAxis();
    float rotation_angle = camera->getAngle();
    MatrixXf cam_rotation = get_rotation_matrix(rotation_axis(0), rotation_axis(1),
            rotation_axis(2), rotation_angle);
    Vector3f cam_dir_transformed = transform_vector(cam_dir, cam_rotation);
    // Don't apply translations to cam_dir
    MatrixXf transform_cam = get_inverse_prod(transformations, true);
    cam_dir_transformed = transform_vector(cam_dir_transformed, transform_cam);

    float e = prm->getExp0();
    float n = prm->getExp1();

    vector<float> t_vals = getInitialGuesses(cam_pos_transformed, cam_dir_transformed);
    vector<float> ans_t_vals;
    for (float initial_t : t_vals) {
        float t = initial_t;
        float sq_io = 100;
        do {
            // Do Newton's method starting at initial_t
            // g(t) = sq_io(ray(t))
            // g'(t) = a * grad sq_io(ray(t))

            Vector3f pos = cam_dir_transformed * t + cam_pos_transformed;
            float x = pos(0);
            float y = pos(1);
            float z = pos(2);

            sq_io = pow((pow((x * x), 1.0 / e) + pow(y * y, 1.0 / e)), e / n)
                + pow(z * z, 1.0 / n) - 1.0;

            Vector3f grad_sq_io;
            grad_sq_io(0) = (2.0 * x * pow(x * x, 1.0 / e - 1.0) *
                    pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e), e / n - 1.0)) / n;
            grad_sq_io(1) = (2.0 * y * pow(y * y, 1.0 / e - 1.0) *
                    pow(pow(x * x, 1.0 / e) + pow(y * y, 1.0 / e), e / n - 1.0)) / n;
            grad_sq_io(2) = (2 * z * pow(z * z, 1.0 / n - 1.0)) / n;

            float deriv = cam_dir_transformed.dot(grad_sq_io);

            // If we're approaching from the outside, deriv should be decreasing
            if (deriv > 0)
                break;

            if (deriv == 0)
                break;

            // Update t
            t -= sq_io / deriv;
        } while (abs(sq_io) > SMALL);

        if (abs(sq_io) <= SMALL)
            ans_t_vals.push_back(t);
    }

    Vector3f* pos_pointer = NULL;

    /*
     * Handle cases for final t vals.
     */
    if (ans_t_vals.size() == 1) {
        float t = ans_t_vals[0];
        if (t > 0) {
            Vector3f pos = cam_dir_transformed * t + cam_pos_transformed;
            pos_pointer = new Vector3f(pos(0), pos(1), pos(2));
        }
    } else if (ans_t_vals.size() == 2) {
        float t1 = ans_t_vals[0];
        float t2 = ans_t_vals[1];
        float t = t1;
        if (t1 > 0 && t2 > 0) {
            t = min(t1, t2);
        } else {
            return NULL; // inside or behind
        }
        Vector3f pos = cam_dir_transformed * t + cam_pos_transformed;
        pos_pointer = new Vector3f(pos(0), pos(1), pos(2));
    }

    // Return UNTRANSFORMED intersection
    return pos_pointer;
}

/*
 * Get initial guesses for Newton's method given the transformed camera position
 * and axis.
 */
vector<float> Assignment::getInitialGuesses(Vector3f cam_pos_transformed,
        Vector3f cam_dir_transformed) {
    // Get starting t for Newton's method
    float a = cam_dir_transformed.dot(cam_dir_transformed);
    float b = 2.0 * cam_dir_transformed.dot(cam_pos_transformed);
    float c = cam_pos_transformed.dot(cam_pos_transformed) - 3.0;

    vector<float> t_vals;

    // Use alt version for t_pos
    float discrim = b * b - 4 * a * c;
    if (discrim < 0)
        return t_vals;

    float t_pos = (2 * c) / (-b - sqrt(discrim));
    float t_neg = (-b - sqrt(discrim)) / (2 * a);

    // Handle cases
    if (t_pos > 0 && t_neg > 0) {
        // Both positive...
        t_vals.push_back(min(t_pos, t_neg)); // Return point closer to cam
    } else if (t_pos < 0 && t_neg < 0) {
        // Both negative...
    } else {
        // One positive, one negative
        t_vals.push_back(t_pos);
        t_vals.push_back(t_neg);
    }

    return t_vals;
}

/*
 * Returns the index of the closest point to the camera. Uses the untransformed
 * camera position, and the points are transformed using the inverse superquadric
 * transformations, so we need to transform the points back to normal.
 */
int Assignment::closestPointIndex(vector<Vector3f> points,
        vector<pair<Primitive*, vector<Transformation>>> pairs, Camera* camera) {
    float min_dist = RAND_MAX;
    int min_index = 0;

    for (int i = 0; i < (int) points.size(); i++) {
        // Untransform point because it has had the inverse superquadric transformations
        // applied
        MatrixXf transform = get_matrix_prod(pairs[i].second);
        Vector3f point = transform_vector(points[i], transform);
        Vector3f diffs = camera->getPosition() - point;
        float dist = diffs.norm();
        if (dist < min_dist) {
            min_dist = dist;
            min_index = i;
        }
    }

    return min_index;
}
