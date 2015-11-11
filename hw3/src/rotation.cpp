#include "rotation.h"
#include <algorithm>

/*
 * Given a starting point (x0, y0) and an ending point (x1, y1) compute
 * the rotation quaternion for the points.
 */
Eigen::Quaternionf compute_rotation_quaternion(int x0, int y0, int x1, int y1,
        int xres, int yres) {
    float rotation_angle = get_rotation_angle(x0, y0, x1, y1, xres, yres);
    Vector3f unit_rotation_axis = get_unit_rotation_axis(x0, y0, x1, y1, xres,
            yres);
    cout << "rotation angle = " << rotation_angle << endl;
    cout << "rotation unit axis: " << endl;
    cout << unit_rotation_axis << endl;

    float quaternion_real = cos(rotation_angle / 2.0);
    float imaginary_scalar = sin(rotation_angle / 2.0);
    Vector3f quaternion_imaginary = unit_rotation_axis * imaginary_scalar;
    Eigen::Quaternionf q(quaternion_real, quaternion_imaginary(0, 0),
            quaternion_imaginary(1, 0), quaternion_imaginary(2, 0));
    return q;
}

/*
 * Computes the unit rotation axis based on the passed-in SCREEN COORDINATE
 * points.
 *
 * Here's what we need to do:
 *
 * Convert screen coordinates to NDC coordinates.
 *
 * Compute z coordinates.
 *
 * Compute unit rotation axis.
 */
Vector3f get_unit_rotation_axis(int x0, int y0, int x1, int y1, int xres,
        int yres) {
    // Convert screen to ndc
    float x0_ndc = screen_to_ndc(x0, xres);
    float y0_ndc = -screen_to_ndc(y0, yres); // OpenGL flips y coords
    float x1_ndc = screen_to_ndc(x1, xres);
    float y1_ndc = -screen_to_ndc(y1, yres); // OpenGL flips y coords

    // Compute z coords
    float z0_ndc = 1 - pow(x0_ndc, 2) - pow(y0_ndc, 2);
    z0_ndc = (z0_ndc > 0) ? sqrt(z0_ndc) : 0;
    float z1_ndc = 1 - pow(x1_ndc, 2) - pow(y1_ndc, 2);
    z1_ndc = (z1_ndc > 0) ? sqrt(z1_ndc) : 0;

    // Compute unit rotation axis
    Vector3f v0_ndc(x0_ndc, y0_ndc, z0_ndc);
    Vector3f v1_ndc(x1_ndc, y1_ndc, z1_ndc);
    Vector3f unit_rotation_axis = v0_ndc.cross(v1_ndc);
    if (unit_rotation_axis.norm() != 0)
        unit_rotation_axis.normalize();
    return unit_rotation_axis;
}


/*
 * Computes the rotation angle based on the passed-in SCREEN COORDINATE
 * points.
 *
 * Here's what we need to do:
 *
 * Convert screen coordinates to NDC coordinates.
 *
 * Compute z coordinates.
 *
 * Compute rotation angle.
 */
float get_rotation_angle(int x0, int y0, int x1, int y1, int xres, int yres) {
    // Convert screen to ndc
    float x0_ndc = screen_to_ndc(x0, xres);
    float y0_ndc = -screen_to_ndc(y0, yres); // OpenGL flips y coords
    float x1_ndc = screen_to_ndc(x1, xres);
    float y1_ndc = -screen_to_ndc(y1, yres); // OpenGL flips y coords

    // Compute z coords
    float z0_ndc = 1 - pow(x0_ndc, 2) - pow(y0_ndc, 2);
    z0_ndc = (z0_ndc > 0) ? sqrt(z0_ndc) : 0;
    float z1_ndc = 1 - pow(x1_ndc, 2) - pow(y1_ndc, 2);
    z1_ndc = (z1_ndc > 0) ? sqrt(z1_ndc) : 0;

    // Compute rotation angle
    Vector3f v0_ndc(x0_ndc, y0_ndc, z0_ndc);
    Vector3f v1_ndc(x1_ndc, y1_ndc, z1_ndc);
    float dot = v0_ndc.dot(v1_ndc);
    dot /= (v0_ndc.norm()) * (v1_ndc.norm());
    float one = 1.0;
    return acos(min(one, dot));
}


/*
 * Convert NDC coordinates to screen coordinates.
 */
float screen_to_ndc(int screen_coord, int screen_res) {
    // Screen to ndc is as follows
    // screen = (ndc + 1) * xres / 2
    // So we just reverse this
    float ndc = (2.0 * screen_coord) / screen_res - 1.0;
    return ndc;
}
