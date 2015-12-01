/* This is a code snippet for drawing the "I-bar". The 'quadratic' object can be made
 * into a global variable in your program if you want. Line 13, where 'quadratic' gets
 * initialized should be done in your 'init' function or somewhere close to the start
 * of your program.
 *
 * The I-bar is an object that Prof. Al Barr once used in one of his papers to demonstrate
 * an interpolation technique. You might call it "Al Barr's I-Bar" ;)
 */

#include <stdio.h>
#include <cmath>
#include <iostream>
#include <cassert>
#include <fstream>
#include <Eigen/Dense>
#include "parser.h"

#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;
using namespace Eigen;

/* Needed to draw the cylinders using glu */
GLUquadricObj *quadratic;

/* Method declarations */
void init();
void display();
void drawIBar();
void key_pressed(unsigned char key, int x, int y);
void init_keyframes();
void init_transformations();
void update_keyframes();
void interpolate_translation_rotation_scaling();
float interpolate_points(Vector4f u_vec, MatrixXf B, Vector4f p_vec);
Quaternionf get_quaternion(Vec4f rotation);
Quaternionf interpolate_quaternions(Quaternionf q1, Quaternionf q2, float u);
Vec4f quaternion2rotation(Quaternionf q);
float deg2rad(float angle);
float rad2deg(float rad);

/* Camera and perspective params (camera angle is 0, so no need for a call
 * to glRotatef for the camera)*/
const float cam_position[] = {0, 0, 40};
const float near_param = 1.0, far_param = 60.0,
            left_param = -1.0, right_param = 1.0,
            bottom_param = -1.0, top_param = 1.0;

/* Arbitrary light */
const float light_color[3] = {1, 1, 1};
const float light_position[3] = {0, 0, -2};

/* Arbitrary material properties for our pendulum */
const float ambient_reflect[3] = {0.3, 0.2, 0.4};
const float diffuse_reflect[3] = {0.7, 0.2, 0.8};
const float specular_reflect[3] = {0.3, 0.3, 0.3};
const float shininess = 0.1;

/* Keyframe vars */
Animation animation;
int frame_num = 0;
int keyframe_num = 0;
Vec3f curr_translation(0, 0, 0);
Vec3f curr_scale(1, 1, 1);
Vec4f curr_rotation(0, 0, 0, 0);
Keyframe prev_frame;
Keyframe curr_frame;
Keyframe next_frame;
Keyframe next_next_frame;

int main(int argc, char* argv[]) {
    // Should be 3 arguments: the keyframe description file, xres, and yres.
    assert (argc == 4);

    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);

    animation = parse_frames(infile);
    animation.print();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(xres, yres);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");

    /* Call our 'init' function...
     */
    init();

    glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    //glutMouseFunc(mouse_pressed);
    //glutMotionFunc(mouse_moved);
    glutKeyboardFunc(key_pressed);
    glutMainLoop();
}

void init()
{
    quadratic = gluNewQuadric();
    init_keyframes();
    init_transformations();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(left_param, right_param,
              bottom_param, top_param,
              near_param, far_param);

    glMatrixMode(GL_MODELVIEW);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Set material properties
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_reflect);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_reflect);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflect);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-cam_position[0], -cam_position[1], -cam_position[2]);
    drawIBar();
    glutSwapBuffers();
}

void drawIBar()
{
    /* Parameters for drawing the cylinders */
    float cyRad = 0.2, cyHeight = 1.0;
    int quadStacks = 4, quadSlices = 4;

    glPushMatrix();

    glTranslatef(curr_translation.x, curr_translation.y, curr_translation.z);
    glRotatef(curr_rotation.alpha, curr_rotation.x, curr_rotation.y, curr_rotation.z);
    glScalef(curr_scale.x, curr_scale.y, curr_scale.z);

    glColor3f(0, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quadratic, cyRad, cyRad, 2.0 * cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(curr_translation.x, curr_translation.y, curr_translation.z);
    glRotatef(curr_rotation.alpha, curr_rotation.x, curr_rotation.y, curr_rotation.z);
    glScalef(curr_scale.x, curr_scale.y, curr_scale.z);

    glColor3f(0, 1, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(curr_translation.x, curr_translation.y, curr_translation.z);
    glRotatef(curr_rotation.alpha, curr_rotation.x, curr_rotation.y, curr_rotation.z);
    glScalef(curr_scale.x, curr_scale.y, curr_scale.z);

    glColor3f(1, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(curr_translation.x, curr_translation.y, curr_translation.z);
    glRotatef(curr_rotation.alpha, curr_rotation.x, curr_rotation.y, curr_rotation.z);
    glScalef(curr_scale.x, curr_scale.y, curr_scale.z);

    glColor3f(1, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(curr_translation.x, curr_translation.y, curr_translation.z);
    glRotatef(curr_rotation.alpha, curr_rotation.x, curr_rotation.y, curr_rotation.z);
    glScalef(curr_scale.x, curr_scale.y, curr_scale.z);

    glColor3f(0, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
}

void key_pressed(unsigned char key, int x, int y) {
    if (key == 'q') {
        // 'q' quits the program
        exit(0);
    } else if (key == 's') {
        // 's' steps forward one frame
        update_keyframes();
        interpolate_translation_rotation_scaling();
        cout << "New frame num = " << frame_num << endl;
        cout << "New translation = " << curr_translation.to_string() << endl;
        cout << "New scale = " << curr_scale.to_string() << endl;
        cout << "New rotation = " << curr_rotation.to_string() << endl;
        cout << endl;
        glutPostRedisplay();
    }
}

/*
 * Initialize Keyframe vars.
 */
void init_keyframes() {
    vector<Keyframe> keyframes = animation.keyframes;
    prev_frame = keyframes[keyframes.size() - 1]; // prev is last elem
    curr_frame = keyframes[0];
    next_frame = keyframes[1];
    next_next_frame = keyframes[2];
}

/*
 * Initialize the initial translation, rotation, and scaling vectors based on
 * the first keyframe.
 *
 * Should be called after init_keyframes().
 */
void init_transformations() {
    curr_translation = curr_frame.translation;
    curr_scale = curr_frame.scale;
    curr_rotation = curr_frame.rotation;
}

/*
 * Update the Keyframe vars needed to do Catmull-Rom interpolation.
 */
void update_keyframes() {
    frame_num = (frame_num + 1) % animation.max_frames;

    if (frame_num == next_frame.frame_num) {
        vector<Keyframe> keyframes = animation.keyframes;

        keyframe_num = (keyframe_num + 1) % keyframes.size();

        prev_frame = keyframes[(keyframe_num - 1) % keyframes.size()];
        curr_frame = keyframes[keyframe_num];
        next_frame = keyframes[(keyframe_num + 1) % keyframes.size()];
        next_next_frame = keyframes[(keyframe_num + 2) % keyframes.size()];
    }
}

/*
 * Use Catmull-Rom to interpolate current translation and scaling vectors.
 *
 * Use SLERP to interpolate rotation vectors.
 */
void interpolate_translation_rotation_scaling() {
    if (frame_num == curr_frame.frame_num) {
        // If we're at a Keyframe, just use the translation and scale vectors
        // of the Keyframe
        curr_translation = curr_frame.translation;
        curr_scale = curr_frame.scale;
        curr_rotation = curr_frame.rotation;
    } else {
        // Else, interpolate
        MatrixXf B(4, 4);
        B << 0, 2, 0, 0,
             -1, 0, 1, 0,
             2, -5, 4, -1,
             -1, 3, -3, 1;
        B *= 0.5;
        float frame_window = (next_frame.frame_num > curr_frame.frame_num) ?
                                (next_frame.frame_num - curr_frame.frame_num) :
                                (animation.max_frames - curr_frame.frame_num);
        float u = (frame_num - curr_frame.frame_num) / frame_window;
        Vector4f u_vec(1, u, u * u, u * u * u);

        // Interpolate translation points
        Vector4f tx_points(prev_frame.translation.x, curr_frame.translation.x,
                next_frame.translation.x, next_next_frame.translation.x);
        curr_translation.x = interpolate_points(u_vec, B, tx_points);
        Vector4f ty_points(prev_frame.translation.y, curr_frame.translation.y,
                next_frame.translation.y, next_next_frame.translation.y);
        curr_translation.y = interpolate_points(u_vec, B, ty_points);
        Vector4f tz_points(prev_frame.translation.z, curr_frame.translation.z,
                next_frame.translation.z, next_next_frame.translation.z);
        curr_translation.z = interpolate_points(u_vec, B, tz_points);

        // Interpolate scale points
        Vector4f sx_points(prev_frame.scale.x, curr_frame.scale.x,
                next_frame.scale.x, next_next_frame.scale.x);
        curr_scale.x = interpolate_points(u_vec, B, sx_points);
        Vector4f sy_points(prev_frame.scale.y, curr_frame.scale.y,
                next_frame.scale.y, next_next_frame.scale.y);
        curr_scale.y = interpolate_points(u_vec, B, sy_points);
        Vector4f z_points(prev_frame.scale.z, curr_frame.scale.z,
                next_frame.scale.z, next_next_frame.scale.z);
        curr_scale.z = interpolate_points(u_vec, B, z_points);

        // Interpolate rotation points
        Quaternionf q1 = get_quaternion(curr_frame.rotation);
        Quaternionf q2 = get_quaternion(next_frame.rotation);
        Quaternionf interpolated_q = interpolate_quaternions(q1, q2, u);
        curr_rotation = quaternion2rotation(interpolated_q);
    }
}

/*
 * Given a vector of points (p_vec), find the interpolated value
 * with the passed-in u_vec and B matrix.
 *
 * Equivalent to evaluating
 * f(u) = uBp
 * at some u.
 */
float interpolate_points(Vector4f u_vec, MatrixXf B, Vector4f p_vec) {
    float interpolated = u_vec.dot(B * p_vec);
    return interpolated;
}

/*
 * Converts a rotation axis and angle into a quaternion.
 */
Quaternionf get_quaternion(Vec4f rotation) {
    Vector3f axis(rotation.x, rotation.y, rotation.z);
    axis.normalize();
    float angle = deg2rad(rotation.alpha);
    float quaternion_real = cos(angle / 2.0);
    float imaginary_scalar = sin(angle / 2.0);
    axis = axis * imaginary_scalar;
    Quaternionf q(quaternion_real, axis(0), axis(1), axis(2));
    return q;
}

/*
 * Use SLERP to interpolate two quaternions.
 */
Quaternionf interpolate_quaternions(Quaternionf q1, Quaternionf q2, float u) {
    float dot = q1.dot(q2);
    float angle = acos(dot);
    float q1_scalar;
    float q2_scalar;
    if (dot >= 1 || dot <= -1) {
        // Here, sin(angle) = 0, so we have q(u) = (1 - u)q1 + uq2
        q1_scalar = 1 - u;
        q2_scalar = u;
    } else {
        q1_scalar = (sin((1 - u) * angle) / sin(angle));
        q2_scalar = (sin(u * angle) / sin(angle));
    }
    Vector4f coeffs = q1.coeffs() * q1_scalar + q2.coeffs() * q2_scalar;
    // coeffs given in order x, y, z, w
    // constructor in order w, x, y, z
    Quaternionf q(coeffs(3), coeffs(0), coeffs(1), coeffs(2));
    return q;
}

/*
 * Converts a Quaternion to its corresponding rotation vector.
 */
Vec4f quaternion2rotation(Quaternionf q) {
    // w is the real component
    float half_angle = acos(q.w());
    float angle = half_angle * 2.0; // in radians
    cout << "angle = " << angle << endl;
    Vector4f coeffs = q.coeffs();
    // Divide all coeffs by sin(angle / 2) to get rotation axis
    coeffs /= sin(half_angle);
    float x = coeffs(0);
    float y = coeffs(1);
    float z = coeffs(2);
    Vec4f rotation(x, y, z, rad2deg(angle));
    return rotation;
}

/*
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle) {
    return angle * M_PI / 180.0;
}

/*
 * Converts given angle in radians to degrees.
 */
float rad2deg(float rad) {
    return rad * 180 / M_PI;
}
