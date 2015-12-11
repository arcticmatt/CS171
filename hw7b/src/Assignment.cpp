#include "Assignment.hpp"

#include "UI.hpp"
#include "Scene.hpp"

#define XRES 250
#define YRES 250

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

/* Ray traces the scene. */
void Assignment::raytrace(Camera camera, Scene scene) {
    cout << "in raytrace" << endl;
    // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
    PNGMaker png = PNGMaker(XRES, YRES);

    // REPLACE THIS WITH YOUR CODE
    // Get camera grid. For each pixel in grid, send out ray. For each ray,
    // see if ray intersects one of the superquadrics. If it does intersect,
    // shade the pixel.
    float near = camera.near;
    Vector3f cam_pos = camera.getPosition();
    float* pos_pointer = new float[3];
    pos_pointer[0] = cam_pos(0);
    pos_pointer[1] = cam_pos(1);
    pos_pointer[2] = cam_pos(2);
    Vector3f e1(0, 0, -1);
    Vector3f e2(1, 0, 0);
    Vector3f e3(0, 1, 0);

    // Solve for width and height
    float tangent = tan(deg2rad(camera.getFov()) / 2.0);
    float height = 2.0 * near * tangent;
    float width = camera.getAspect() * height;

    for (int i = 0; i < XRES; i++) {
        for (int j = 0; j < YRES; j++) {
            float x = ((float) i - (XRES / 2.0)) * width;
            float y = ((float) j - (YRES / 2.0)) * height;
            //cout << "x = " << x << endl;
            //cout << "y = " << y << endl;

            Vector3f axis = near * e1 + x * e2 + y * e3;
            cout << "axis = " << axis << endl;
            float* axis_pointer = new float[3];
            axis_pointer[0] = axis(0);
            axis_pointer[1] = axis(1);
            axis_pointer[2] = axis(2);
            Camera* curr_cam = new Camera(pos_pointer, axis_pointer,
                    camera.angle, camera.near, camera.far, camera.fov, camera.aspect);

            Vector3f* normal = getIntersectNormal(curr_cam, scene);
            if (normal != NULL) {
                png.setPixel(i, j, 1.0, 0, 0);
                cout << "normal = \n" << *normal << endl;
            } else {
                png.setPixel(i, j, 1.0, 1.0, 1.0);
            }
        }
    }

    cout << "Done iterating" << endl;

    // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
    if (png.saveImage())
        printf("Error: couldn't save PNG image\n");
}

// From part A
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
 * Gets the normal of the superquadric that intersects with the ray of the
 * passed-in camera, or null if there is no intersection.
 */
Vector3f* Assignment::getIntersectNormal(Camera* camera, Scene scene) {
    // Populate scene copy with list of primitives and transformations
    scene_copy.clear();

    for (Object *obj : scene.root_objs)
        traverseRen(obj, vector<Transformation>(), 0);

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
        Vector3f *norm_pointer = new Vector3f(norm(0), norm(1), norm(2));
        return norm_pointer;
    }

    return NULL;
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

    // Transform norm
    MatrixXf transform = get_matrix_prod(transformations);
    norm = transform_vector(norm, transform);

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
MatrixXf get_matrix_prod(vector<Transformation> transformations,
        bool exclude_translation) {
    MatrixXf prod = MatrixXf::Identity(4, 4);
    MatrixXf curr_mat;
    for (int i = (int) transformations.size() - 1; i >= 0; i--) {
        if (exclude_translation && transformations[i].type == TRANS)
            continue;
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
 * Converts degrees to radians.
 */
float deg2rad(float angle) {
    return angle * M_PI / 180.0;
}
